
#ifndef KEY
#error undefined macro KEY
#endif

KEY( KEY_NUM0 	, 0x0080 )// 数字0

KEY( KEY_NUM4	, 0x0100  )
KEY( KEY_NUM5	, 0x0200  )
KEY( KEY_NUM6	, 0x0400 )
KEY( KEY_NUM7	, 0x0010 )
KEY( KEY_NUM8	, 0x0020 )
KEY( KEY_NUM9	, 0x0040 )
	
KEY( KEY_NUM1	, 0x8000 )	// 数字1 
KEY( KEY_NUM2	, 0x4000 )	
KEY( KEY_NUM3	, 0x2000 )
KEY( KEY_LOCK	, 0x1000 )		// 锁定
	
KEY( KEY_UNLOCK	, 0x0800 )		// 解锁
	
KEY( KEY_SWITCH	, 0x0008 )		// 开关
KEY( KEY_SET	, 0x0004 )		// 设置
KEY( KEY_OK		, 0x0002 )			// 确定
KEY( KEY_BACK	, 0x0001 )		// 返回

#undef KEY
