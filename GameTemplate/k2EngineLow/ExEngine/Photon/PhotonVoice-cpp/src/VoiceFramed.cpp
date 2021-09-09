/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/VoiceClient.h"
#include "PhotonVoice-cpp/inc/VoiceFramed.h"

/** @file PhotonVoice-cpp/inc/VoiceFramed.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		/// <summary>Create new Framer instance.</summary>
		template<typename T>
		Framer<T>::Framer(int frameSize)
			: mFrame(frameSize)
			, mFramePos(0)
		{
		}

		template<typename T>
		Framer<T>::~Framer(void)
		{
		}

		/// <summary>Gets the number of frames available after adding bufLen samples.</summary>
		/// <param name="bufLen">Number of samples that would be added.</param>
		/// <returns>Number of full frames available when adding bufLen samples.</returns>
		template<typename T>
		int Framer<T>::count(int bufLen) const
		{
			return (bufLen + mFramePos)/mFrame.Length;
		}

		/// <summary>Appends the provided buffer to any preexistent samples from a previous call (if any), frames the result of the addition and calls the provided callback once for each full frame, until there are not enough remaining samples to fill a complete frame,
		/// then stores any remaining samples for the next call. </summary>
		/// <param name="buf">Array of samples to add.</param>
		/// <param name="opaque">A pointer to custom data that should be forwarded to the callback.</param>
		/// <param name="op">The callback to call.</param>
		template<typename T>
		void Framer<T>::frame(const Buffer<T>& buf, void* opaque, void(*op)(void*, const Buffer<T>&))
		{
			// quick return in trivial case
			if(mFrame.getSize() == buf.getSize() && !mFramePos)
				op(opaque, buf);
			else
			{
				int bufPos = 0;
				while(mFrame.getSize() - mFramePos <= buf.getSize() - bufPos)
				{
					int l = mFrame.getSize() - mFramePos;
					memcpy(mFrame.getArray() + mFramePos, buf.getArray() + bufPos, l*sizeof(T));
					bufPos += l;
					mFramePos = 0;
					op(opaque, mFrame);
				}
				if(bufPos != buf.getSize())
				{
					int l = buf.getSize() - bufPos;
					memcpy(mFrame.getArray() + mFramePos, buf.getArray() + bufPos, l*sizeof(T));
					mFramePos += l;
				}
			}
		}

		// Optionally process input data. 
		// Should return arrays exactly of info.FrameSize size or null to skip sending
		template<typename T>
		Buffer<T> LocalVoiceFramed<T>::processFrame(const Buffer<T>& buf)
		{
			Lockguard lock(mProcessors);
			Buffer<T> processedBuf = buf;
			for(unsigned int i=0; i<mProcessors.getSize(); i++)
			{
				processedBuf = mProcessors[i]->process(processedBuf);
				if(!processedBuf.getSize())
					break;
			}
			return processedBuf;
		}

		/// <summary>
		/// Adds the processor that is provided as a parameter after any built-in processors and everything added with addPreProcessor().
		/// </summary>
		/// <param name="processor"></param>
		template<typename T>
		void LocalVoiceFramed<T>::addPostProcessor(SharedPointer<IProcessor<T> > processor)
		{
			Lockguard lock(mProcessors);
			mProcessors.addElement(processor);
		}

		/// <summary>
		/// Adds the processor that is provided as a parameter before the built-in processors and everything added with addPostProcessor().
		/// </summary>
		/// <param name="processor"></param>
		template<typename T>
		void LocalVoiceFramed<T>::addPreProcessor(SharedPointer<IProcessor<T> > processor)
		{
			Lockguard lock(mProcessors);
			mProcessors.insertElementAt(processor, mPreProcessorsCnt++);
		}

		/// <summary>
		/// Clears all processors in pipeline including built-in resampling.
		/// User should add at least resampler processor after call.
		/// </summary>
		template<typename T>
		void LocalVoiceFramed<T>::clearProcessors(void)
		{
			Lockguard lock(mProcessors);
			mProcessors.removeAllElements();
			mPreProcessorsCnt = 0;
		}

		template<typename T>
		LocalVoiceFramed<T>::LocalVoiceFramed(VoiceClient* pVoiceClient, SharedPointer<IEncoder> encoder, nByte id, const VoiceInfo& voiceInfo, int channelId, int frameSize)
			: LocalVoice(pVoiceClient, encoder, id, voiceInfo, channelId)
			, mFrameSize(frameSize)
			, mFramer(frameSize)
			, mPreProcessorsCnt(0)
			, mEncodeThreadStarted(false)
			, mExitDecodeThread(false)
			, mFramesSkippedNextLog(0)
			, mFramesSkipped(0)
		{
		}

		template<typename T>
		int LocalVoiceFramed<T>::getFrameSize(void) const
		{
			return mFrameSize;
		}

		template<typename T>
		LocalVoiceFramed<T>::~LocalVoiceFramed(void)
		{
			mExitDecodeThread = true;
			mPushDataQueueReady.set();
			while(mEncodeThreadStarted);
		}

		/// <summary>Wether this LocalVoiceFramed has capacity for more data buffers to be pushed asynchronously.</summary>
		template<typename T>
		bool LocalVoiceFramed<T>::isPushDataAsyncReady(void)
		{
			Lockguard lock(mPushDataQueue);
			return mPushDataQueue.getSize() < DATA_POOL_CAPACITY - 1;  // 1 slot for buffer currently processed and not contained either by pool or queue
		}

		template<typename T>
		void LocalVoiceFramed<T>::encodeThreadStarter(void* pArgs)
		{
			LocalVoiceFramed<T>* pV = static_cast<LocalVoiceFramed<T>*>(pArgs);
			pV->pushDataAsyncThread();
		}

		/// <summary>Asynchronously push data into this stream.</summary>
		// Accepts array of arbitrary size. Automatically splits or aggregates input to buffers of length <see cref="FrameSize"></see>.
		// Expects buf content to be preserved until PushData is called from a worker thread. Releases buffer to <see cref="BufferFactory"></see> then.
		template<typename T>
		void LocalVoiceFramed<T>::pushDataAsync(const Buffer<T>& buf)
		{
			if(!mEncodeThreadStarted)
			{
				mEncodeThreadStarted = true;
				mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Starting data encode thread");
				Thread::create(encodeThreadStarter, this);
			}

			// Caller should check this asap in general case if packet production is expensive.
			// This is not the case For lightweight audio stream. Also overflow does not happen for audio stream normally.
			// Make sure that queue is not too large even if caller missed the check.
			if(isPushDataAsyncReady()) // could inline this mehod and save 1 lock if it makes sense for spinlocks
			{
				{
					Lockguard lock(mPushDataQueue);
					mPushDataQueue.enqueue(buf);
				}
				mPushDataQueueReady.set();
			}
			else
			{
				if(mFramesSkipped == mFramesSkippedNextLog)
				{
					mpVoiceClient->getTransport()->logWarning(getLogPrefix() + L": PushData queue overflow. Frames skipped: " + (mFramesSkipped + 1));
					mFramesSkippedNextLog = mFramesSkipped + 10;
				}
				mFramesSkipped++;
			}
		}

		template<typename T>
		void LocalVoiceFramed<T>::pushDataAsyncThread(void)
		{
			while(!mExitDecodeThread)
			{
				mPushDataQueueReady.waitOne(); // Wait until data is pushed to the queue or Dispose signals.

				bool empty = false;
				while(!empty) // Dequeue and process while the queue is not empty
				{
					if(mExitDecodeThread)
						break; // early exit to save a few resources

					Buffer<T> b;
					{
						Lockguard lock(mPushDataQueue);
						if(mPushDataQueue.getSize() > 0)
							b = mPushDataQueue.dequeue();
						else
							empty = true;
					}
					if(!empty)
						pushData(b);
				}
			}
			mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Exiting data encode thread");
			mEncodeThreadStarted = false;
		}

		template<typename T>
		void LocalVoiceFramed<T>::framerOp(void* opaque, const Buffer<T>& framed)
		{
			LocalVoiceFramed<T>* pV = static_cast<LocalVoiceFramed<T>*>(opaque);
			Buffer<T> processed = pV->processFrame(framed);
			if(processed.getSize() && !pV->mspEncoder->getError().length())
				static_cast<IEncoderDirect<Buffer<T> >*>(&*(pV->mspEncoder))->input(processed);
		}

		/// <summary>Synchronously push data into this stream.</summary>
		// Accepts array of arbitrary size. Automatically splits or aggregates input to buffers of length <see cref="FrameSize"></see>.
		template<typename T>
		void LocalVoiceFramed<T>::pushData(const Buffer<T>& buf)
		{
			if(mpVoiceClient->getTransport()->isChannelJoined(mChannelId) && getTransmitEnabled())
				mFramer.frame(buf, this, framerOp);
		}

		template class LocalVoiceFramed<float>;
		template class LocalVoiceFramed<short>;
	}
}