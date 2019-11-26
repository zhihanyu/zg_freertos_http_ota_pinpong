# 系统功能


## 工程适配

> ZG_system.h：
>
> - 对应平台头文件包含；
> - #define ZG_system_time()  对应平台系统时间　　#系统时间适配
>
> store.c：
>
> - ZG_store_fast() 和 static void store_task(void *arg) 　#需要适配对应平台的保存函数
>
> ota.c
>
> - void ota_task(void *arg)  #　适配对应平台ota的api
>

### 代码结构
```
.
├── AT_CMD.c                   # AT指令
├── RTC_time.c                 # RTC时间
├── ZG_system.c               　# 系统事件
├── encyption.c            　　 　# AES加密功能
├── fota.c               　　　　　　# 在线升级功能
├── store.c               　　　　　# flash保存功能
└── readme.md                  # 说明文档


## 参考文档

> 文档资源

1. 《2.江门模块AES功能及要求0421_New》
2. 《我们使用到的AT指令1》
