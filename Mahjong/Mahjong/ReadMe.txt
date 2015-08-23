愉快的日本麻将游戏实现。

本实现将注重于比赛功能，顺手解决一下国际网络连接问题，目标是为各大联赛提供一个更好的比赛环境，避免网络决定雀力的悲剧发生。

2015.8.24

1、修复了 TenpaiAkariJudge 模块中数个 Bug。
(1) 第二次消面子迭代中，分配 0 字节空间的问题。
(2) 在部分情况下，tenpai_detect 的返回值 judgeResult.t 中有成员无效。
2、增加了判断进张的功能 (Syanten.kouritsuDetect)。
3、增加了从天凤手牌字符串转换为牌数组的功能 (convertPaiString / convertPaiStringPrepare)。
4、在主程序中写了向听计算和听牌计算的功能。