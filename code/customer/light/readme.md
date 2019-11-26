# 灯光功能


## 工程适配

> Light_func.c：
>
> - PWM_init()　#PWM　配置适配
>
> Light_func.h：
>
> - #define  dimming(channel,val)  #pwm调光适配
>
> IR.c
>
> - IR_Init()  #IR初始化:IO口下降沿中断,开启10us的定时器中断
>

### 代码结构
```
.
├── IR.c                     　　 # 红外解码
├── Light_fun.c                 # 灯光功能
├── Remote.c               　　　　　# 红外遥控功能
├── auto_report.c            　　　# 灯光状态自动上报到服务器功能
├── clock.c               　　　　　　# 闹钟功能
├── color.c               　　　　　　# hsv转换算法
└── readme.md                   # 说明文档


## 参考文档

> 文档资源

1. 《WIFI Mini RGB 控制器通讯协议》
2. 《WiFi 产品通用通信协议》
