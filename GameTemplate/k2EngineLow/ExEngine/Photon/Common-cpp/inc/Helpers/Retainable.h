/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Common.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			template<typename T>
			class Retainable : public T
			{
			public:
				Retainable(void);
				virtual Retainable<T>* retain(void);
				virtual void release(void);

				virtual unsigned char getRefCount(void);
			protected:
				virtual ~Retainable(void);
			private:
				virtual void dealloc(void);

				unsigned char mRefCount;
			};



			template<typename T>
			Retainable<T>::Retainable(void)
				: mRefCount(0)
			{
				retain();
			}

			template<typename T>
			Retainable<T>::~Retainable(void)
			{
			}

			template<typename T>
			Retainable<T>* Retainable<T>::retain(void)
			{
				++mRefCount;
				return this;
			}

			template<typename T>
			void Retainable<T>::release(void)
			{
				if(!--mRefCount)
					dealloc();
			}

			template<typename T>
			unsigned char Retainable<T>::getRefCount(void)
			{
				return mRefCount;
			}

			template<typename T>
			void Retainable<T>::dealloc(void)
			{
				DEALLOCATE(Retainable<T>, this);
			}
		}
	}
}