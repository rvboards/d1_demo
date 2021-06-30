### RVBoards D1 RGB炫彩点灯dome


跑在 RVBoards D1 单板机上的RGB炫彩点灯dome。读 /proc/stat 获取 CPU 使用率，写 /sys/class/leds/sunxi_led0[rgb]/brightness 点亮RGB灯

基于大佬的  [RVBoards D1 CPU 使用率指示器](http://gitee.com/zoomdy/rvboards_d1_cpu_usage_indicator/tree/master)  修改而来，有兴趣的可以看看。

此程序主要在gitee同步 [网址点击这里](https://gitee.com/weizhiunknown/rvboards-d1-rgb)


编译后：

 **_任一色值不要大于255_** 


### ./brvboardsRGB
	

        直接运行：每20ms打印一次并且自动进行彩虹色变换。

### ./brvboardsRGB end


        熄灭



### ./brvboardsRGB r [可选R色值]


        无色值时红灯125 0 0

        有色值时为对应色值，注意无保护，不要大于255

### ./brvboardsRGB g [可选G色值]


        无色值时绿灯125 0 0

        有色值时为对应色值，注意无保护，不要大于255
	
### ./brvboardsRGB b [可选B色值]


        无色值时蓝灯125 0 0

        有色值时为对应色值，注意无保护，不要大于255	



### ./brvboardsRGB rgb [可选RGB色值，每个值间用空格隔开]


        必须有色值，注意无保护，任一色值不要大于255
