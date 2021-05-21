rollout tkExporter "?^?C?g?????" width:182 height:597
(
	editText 'animClipStartFrame' "" pos:[90,462] width:58 height:21 align:#left
	label 'lbl1' "start frame" pos:[22,465] width:60 height:16 align:#left
	groupBox 'grp1' "Animation clip" pos:[14,436] width:147 height:134 align:#left
	button 'btn_saveAnimationClip' "Save" pos:[27,531] width:120 height:28 align:#left
	label 'lbl2' "end frame" pos:[22,496] width:60 height:16 align:#left
	editText 'animClipEndFrame' "" pos:[90,494] width:58 height:21 align:#left
	button 'btn_saveSkeleton' "Save Skeleton" pos:[22,399] width:120 height:28 align:#left
	label 'lbl3' "Label" pos:[33,458] width:0 height:0 align:#left
	groupBox 'grp5' "Model" pos:[3,13] width:164 height:138 align:#left
	button 'btn_save_fbx' "Save(*.tkm)" pos:[27,33] width:120 height:28 align:#left
	groupBox 'grp10' "Level" pos:[3,159] width:164 height:211 align:#left
	button 'btn_load_locObject' "Load Object" pos:[26,250] width:120 height:28 align:#left
	editText 'levelName' "" pos:[23,216] width:120 height:24 align:#left
	label 'lbl4' "edit level name" pos:[23,195] width:121 height:16 align:#left
	button 'SaveLoc' "Save Level" pos:[26,320] width:120 height:28 align:#left
	groupBox 'grp6' "Animation" pos:[3,373] width:164 height:200 align:#left
	button 'btn_copy_locObject' "Copy Object" pos:[26,284] width:120 height:28 align:#left
	button 'btn_save_selected_fbx' "Save Selected(*.tkm)" pos:[27,79] width:120 height:28 align:#left
	checkButton 'ckb1' "CheckButton" pos:[47,133] width:0 height:0 align:#left
	checkbox 'flat shading' "flat shading" pos:[27,116] width:128 height:29 align:#left
)