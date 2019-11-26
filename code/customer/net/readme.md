# 网络功能


## 工程适配

> WIFI_config.c 
>
> - void wifi_scan_ap()　#wifi扫描附近ap功能适配
>
> - ZG_restart_system() #重启系统功能适配
>
> - wifi callback event  #wifi连接事件适配
>
> -  void wifi_STA_Start()　// STA功能适配
>
> - void wifi_AP_Start() //　AP功能适配
>
> - void wifi_get_ip_info(char *ip_addr) //适配ip地址：AP模式：10.10.123.3 ,sta模式ip由路由器分配
>
> - int get_ap_rssi() //适配连接的路由器信号
>
> - void wifi_get_mac_info(char *mac_str)　//适配mac地址


### 代码结构
```
.
├── WIFI_config.c              # 网络配置
└── readme.md                  # 说明文档


## 参考文档

> 文档资源


