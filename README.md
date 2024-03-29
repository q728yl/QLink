# QLink
基于Qt的RPG类连连看小游戏

在连连看游戏中，会有⼀个地图，地图上有许多不同种类的⽅块，通过将相同种类的两个⽅块相连，可以将这两个⽅块消除，⽤户获得分数。
在整个连连看的过程中，除了处理⽤户的操作之外，还有⼏个⽐较特殊的部分特别需要注意：
# 随机地图的⽣成
判断两个⽅块是否可以通过两次以内的折线进⾏连接
判断剩余⽅块是否还有解
除此之外，具体的功能要求如下：
# RPG 机制 
不同于传统的连连看，我们的 QLink 使⽤ RPG 模式进⾏，即玩家需要控制⼀个⻆⾊在地图的空地上移动（⻆⾊显示可⾃⾏选择）。
激活：当⻆⾊处于⽅块旁且再次向⽅块⽅向移动，会激活该⽅块（请使⽤某种⽅式表示该⽅块被激活）。
消除：如果此次激活的⽅块和上次激活的⽅块是同种类，且可以通过两次以内的折线连接，则该两个⽅块被消除，玩家获得分数。（请绘制出将两个⽅块连接在⼀起的折线） 否则，上次激活的⽅块被⾃动变为未激活状态，换句话说，每个⻆⾊在地图中只有 0 个（游戏刚开始时，或刚刚消除完⼀对⽅块时）或者 1 个激活的方块。
# 计分 
不同种类的⽅块可以有不同的分值，具体规则可以⾃⾏制定。界⾯中应时刻显示玩家的分数。
# 倒计时和游戏结束 
有两个情况可以导致游戏结束：
1. 倒计时结束；
2. 没有可消除的⽅块对（所有⽅块均被消除也属于这⼀种）。
界⾯中应时刻显示游戏的倒计时。
# 开始菜单 
⾄少包括以下按钮：
开始新游戏
可选择游戏模式：单⼈模式、双⼈模式（具体看后⽂）
载⼊游戏
退出游戏
单⼈模式 
游戏开始时，会随机⽣成地图，并随机玩家⻆⾊位置。随后玩家可控制⻆⾊移动，以激活和消除⽅块。
# 道具 
道具通过随机⽅式出现在地图的空地上，当⻆⾊与道具出现在同⼀位置时，该⻆⾊触发道具效果，道具消失。
+1s：延⻓剩余时间 30s
 Shuffle：所有⽅块位置重排
Hint：10s 内会⾼亮⼀对可能链接的⽅块，被消除后会⾼亮下⼀对，直到 10s 时间结束
Flash：5s 内允许通过⿏标单击移动⻆⾊位置，⻆⾊移动到的位置必须通过空地可到达，否则点击不产⽣任何效果。如果点击到⽅块，且⻆⾊可以移动到该⽅块旁，则⻆⾊移动到该⽅块旁，且该⽅块被激活。如果⽅块四
周有多个位置可以让⻆⾊停留，则⻆⾊移动到其中任何⼀个位置均可。
# 双人模式 
两个玩家的两个⻆⾊在相同的地图上进⾏游戏，以结束游戏时双⽅的分数决定谁为赢家。
道具在单⼈模式的基础上，增加：
Freeze：对⼿ 3s 内⽆法移动
Dizzy：对⼿ 10s 内移动⽅向颠倒（上下左右颠倒）
此外，
Hint 道具的效果对两个玩家均可⻅；
+1s 道具的效果对两个玩家均有效。
双⼈模式下，没有 Flash 道具
# 暂停和存档 
在暂停时，可以保存游戏（Save）和载⼊游戏（Load）
保存游戏会将当前游戏的所有状态以任意格式保存到磁盘上的⽂件
载⼊游戏时，读取⽂件，并从中恢复状态
