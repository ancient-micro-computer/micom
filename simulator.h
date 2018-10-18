/*============================================================================*
					仮想システムコンフィギュレーション

　システムで使用するモジュールや周辺I/O等のデバイスマッピングを定義しています。

 *============================================================================*/

// バス接続モジュールのID
#define D_MODULEID_CPU		0
#define D_MODULEID_GTMR		1
#define D_MODULEID_DMA		2
#define D_MODULEID_MEM		10

// メモリマップ範囲定義
#define D_ADRMAP_SYSTEM_START	0x0000
#define D_ADRMAP_SYSTEM_END		0x0FFF
#define D_ADRMAP_USR_START		0x1000
#define D_ADRMAP_USR_END		0xEFFF
#define D_ADRMAP_STACK_START	0xF000
#define D_ADRMAP_STACK_END		0xFFFF

// メモリマップ詳細定義
#define D_SYSTEM_IPL			(D_ADRMAP_SYSTEM_START)
#define D_SYSTEM_INTVECT		(D_SYSTEM_IPL + 0x10)
#define D_SYSTEM_GTMR_ENABLE	(D_SYSTEM_INTVECT + 0x8)
#define D_SYSTEM_GTMR_INTERVAL	(D_SYSTEM_INTVECT + 0x9)

#define D_SYSTEM_PORT_BASE		(0x0100)
#define D_SYSTEM_LED0			(D_SYSTEM_PORT_BASE + 0)
#define D_SYSTEM_LED1			(D_SYSTEM_PORT_BASE + 1)
#define D_SYSTEM_LED2			(D_SYSTEM_PORT_BASE + 2)
#define D_SYSTEM_LED3			(D_SYSTEM_PORT_BASE + 3)
#define D_SYSTEM_LED4			(D_SYSTEM_PORT_BASE + 4)
#define D_SYSTEM_LED5			(D_SYSTEM_PORT_BASE + 5)
#define D_SYSTEM_LED6			(D_SYSTEM_PORT_BASE + 6)
#define D_SYSTEM_LED7			(D_SYSTEM_PORT_BASE + 7)

#define D_SYSTEM_SLIDER0		(D_SYSTEM_PORT_BASE + 16)
#define D_SYSTEM_SLIDER1		(D_SYSTEM_PORT_BASE + 17)
#define D_SYSTEM_SLIDER2		(D_SYSTEM_PORT_BASE + 18)
#define D_SYSTEM_SLIDER3		(D_SYSTEM_PORT_BASE + 19)
#define D_SYSTEM_SLIDER4		(D_SYSTEM_PORT_BASE + 20)
#define D_SYSTEM_SLIDER5		(D_SYSTEM_PORT_BASE + 21)
#define D_SYSTEM_SLIDER6		(D_SYSTEM_PORT_BASE + 22)
#define D_SYSTEM_SLIDER7		(D_SYSTEM_PORT_BASE + 23)
