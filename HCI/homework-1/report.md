# 调研报告

## Google

### 技术基础

Google 近十年推出了很多智能化的交互产品，我认为其背后的技术基础有两个主要方面，一是 Android 操作系统，而是 Google 的 AI 生态。

#### Android

Android 是一个基于 Linux 内核的开源移动操作系统，由 Google 成立的开放手持设备联盟持续领导与开发，主要设计用于触摸屏移动设备与其他便携式设备。而新一代的智能化产品正是应用于这类设备之上。

Android 的主要硬件平台为 ARM 架构，相比于在更高端版本中也支持的 x86 架构，ARM 架构更适合应用于移动设备上。Android 设备还包括了许多可选的硬件部件，比如视频录像机、方向传感器、专用游戏控制器、气压计、触摸屏等等。

软件方面，Android 内核是 Linux 内核长期支持的分支，具有典型的 Linux 调度和功能。Android 内核进一步实现了在典型的 Linux 内核开发周期之外的架构变化，具体表现为去除了 Linux 中的本地 X Window System，也不支持标准 GNU 库，但这也使得 Linux 平台上的应用程序移植到 Android 平台上变得更加困难。

Android 的核心基于 Linux ，核心之外，则是中介层、数据库元和用 C/C++ 编写的 API 以及应用程序框架。Android 的应用程序通常以 Java 数据库元为基础编写，运行程序时，应用程序的代码会被即时转变为 Dalvik dex-code（Dalvik Executable），然后 Android 操作系统通过使用即时编译的 Dalvik 虚拟机来将其运行。

#### AI 生态

和百度的 All In AI 相似，Google 也在 2016 年前后提出了 AI First 的战略，AI First 是 Mobile First 的延续，因为其背后的基本逻辑是计算的普适化，大量的智能设备（包括移动智能设备）和传感器出现在日常生活中。AI First的背后是未来几十年会是包括移动设备在内的智能设备的大量普及。

同时，Google 的 AI 生态已然形成一个紧密的闭环，从高效运行神经网络计算的专用硬件 TPU，到深度学习框架 TensorFlow，Google 在 AI 技术的软硬件领域都有涉足，近年来 TensorFlow 更是和云原生的概念相结合，打造出了 Kubeflow 这样面向 Kubernetes 的机器学习工具包。

但是 AI 生态中仅仅有技术是不够的，数据同样至关重要。Google 的数据从何而来，我想 Google 一系列产品如 YouTube、Google Map 每日以 PB 计的数据量能够回答这一问题，以这些数据构建出的 Google Knowledge Graph 成为了支撑 AI 生态的知识图谱。

### 创新研究

#### Android Nougat

Android Nougat 是 Android 操作系统的第 7 个主要版本，为什么它在众多的 Android 版本中值得一提呢，因为它引入了操作系统及其开发平台显著的变化，包括在屏幕上同时显示多个应用程序的能力，内联通知恢复的支持，以及一个基于 OpenJDK 的 Java 的环境，对于先进的 Vulkan 2D/3D 图形渲染 API 的支持，并支持设备“无缝”系统更新。

除此以外，它的出现大大改进了 Android 设备的用户体验，例如随时随地激活休息模式、自定义快速设置、在通知栏直接回复、流畅更新等等。

#### Wear OS

Wear OS 是 Android 操作系统的一个分支版本，专为智能手表等可穿戴设备所设计。它的出现更好地支持了可穿戴设备优秀的交互体验，例如用户可以通过语音使用 Google Assistant，并随时搜索解答或获取信息。此外它还为设备提供了传感器支持，可用于加速度计与脉搏监视器等应用程序。

#### Google Assistant

Google Assistant 是 Google 开发的智能个人助理，与它的前身 Google Now 不同，它可以参与双向对话。它提供很多强大的功能，例如多语言的语音识别、安排行程与闹铃、调整用户设备的硬件显示、搜索网络等等，而他最大的不同就在于使用了 Google 的自然语言处理介入双向对话，真正做到了类似于管家这样的角色。

Google Assistant 当前也被集成到了 Google Allo 应用、Google Home 设备、以及支持了 Android Nougat 或者 Wear OS 的设备上。PC World 的 Mark Hachman 赞扬它为“Cortana 和 Siri 的升级版”。

#### Google Allo

Google Allo 是 Google 开发的一款即时通讯应用，内置了虚拟助手 Google Assistant，提供了智能回复、隐身模式等功能。

#### Google Home

Google Home 是 Google 推出的一系列智能音箱。这些设备使用户能够通过语音命令与 Google Assistant 进行一系列交互，如播放音乐、收听语音新闻等操作。Google Home 系列设备还集成了对智慧家庭的支持。

#### Google Knowledge Map

Google Knowledge Map 是 Google 的知识库，也是其 AI 生态中十分重要的一环，其使用语义检索从多种来源收集信息。知识图谱除了显示其他网站的链接列表，还提供结构化及详细的关于主题的信息。其目标是，用户将能够使用此功能提供的信息来解决他们查询的问题，而不必导航到其他网站并自己汇总信息。

#### Google Lens

Google Lens 是 Google 在 2017 年 Google I/O 上发布的一款应用程序，旨在通过视觉分析提供相关信息。当用手机相机拍摄某个物体时，Google Lens 会尝试识别该物体并显示相关搜索结果和信息。例如，将设备的摄像头指向包含网络名称和密码的 Wi-Fi 标签时，它会自动连接到已扫描的 Wi-Fi 网络。有点类似于扫一扫。

#### Android TV

Android TV 是 Google 推出的专为数字媒体播放器所设计的 Android 分支版本。它提供一套以内容发现、语音搜索为特色的用户界面，能够集成来自不同媒体服务和应用的内容，并能够与Google的其他产品联动，例如 Google Assistant、Google Cast。

#### Google Cast

Google Cast 是 Google 的一项服务，用于将支持 Google Cast 的应用程序（如 YouTube）的画面投放到 Android TV 上。

## Apple

### 技术基础

#### iOS

说到苹果公司的技术基础，iOS 一定是绕不开的。iOS 是苹果公司为其移动设备所开发的操作系统。

iOS 的用户界面使用多点触控来直接操作，界面操控元素包括滑动条、开关及按钮，与操作界面的交互包括滑动、点击、扭捏及反向扭捏。可以说 iOS 本身就是人机交互的一个优秀案例。

相比于技术基础，不论是 Apple 产品的设计质感，还是用户黏着度，都是其成功背后不可或缺的因素。

### 创新研究

#### Siri

Siri（Speech Interpretation and Recognition Interface）是一款内置在苹果 iOS 系统中的人工智能助理软件。此软件使用自然语言处理技术，用户可以使用自然的对话与手机进行交互，完成搜索数据、查询天气、设置手机日历、设置闹铃等许多服务。

#### AirPods

AirPods 是苹果公司推出的蓝牙无线耳机，于 2016 年 9 月 7 日苹果秋季新品发布会上初次发布，不过该耳机因为开发原因导致延迟到 2016 年 12 月中才正式对外发售，而且因为供不应求，当时之运送等待时间达到至少 6 周之久。

AirPods 的感应器分别有双波束形成麦克风、双光学感应器、动态识别感应器、话音识别感应器。

AirPods 使用蓝牙传输协议，内置光学传感器及加速仪，配对完成后戴上即自动引导，拿下后自动停止播放。

#### HomePod

HomePod 是由苹果公司开发的智能音箱，于 2017 年苹果公司全球软件开发者年会时发布。

HomePod 使用 Siri 作为其虚拟助理，不仅支持 Apple Music，也可通过支持隔空播放 2 的应用流化音频，并可支持 HomeKit，其会自动将自己设置为家居中枢以提供智能家庭及远程访问服务。

#### Apple Wallet

Apple Wallet 是苹果公司 iOS 中的一个应用程序，让用户可以在手机中存储优惠券、登机牌、活动门票、回馈卡或其他类型的移动支付凭证等。此外，苹果公司运营的支持非接触式支付的支付服务 Apple Pay 的支付卡也集成在此应用程序中。

#### Apple TV

Apple TV 是一款由苹果公司所设计、营销和销售的数字多媒体机顶盒。它可用作播放来自 iTunes Store、Netflix、YouTube、Flickr、MobileMe 里的在线内容或电脑上 iTunes 里的多媒体文件，并透过高分辨率宽屏幕的电视机输出影像。

第三代 Apple TV 由于加入了 A5 芯片，因此当前已经可以播放 1080p 全高清电影。

#### Apple Watch

Apple Watch 是苹果公司开发的一款智能手表，由蒂姆·库克于 2014 年 9 月 9 日发布。Apple Watch 结合了运动追踪和健康相关功能，并能与 iOS 和其他苹果产品与服务集成。

Apple Watch 支持移动支付平台 Apple Pay。此外，手表在与 iPhone 链接后可拨打和接听电话、发送和阅读 iMessage 和 SMS 短信、追踪健康及运动状况、运行第三方开发的应用程序。苹果手表能控制 Apple TV，并可当作对讲机及 iPhone 相机的取景窗使用。透过手表中内置的地图应用程序，可提供穿戴者导航与方向指引。此外手表也能在 Passbook 中存储会员卡、入场门票和登机证等票卷。苹果手表中也包含了数字个人助理 Siri 功能。手表采用了力道感应触控的输入方式，能感测穿戴者按压屏幕的力道大小。

## 微软

### 技术基础

和 Google，Apple 类似，微软公司试图在其产品周围创建“生态体系”，以为其产品以及品牌增值。

软件产品诸如 Windows 操作系统，Microsoft Office 套装软件，Microsoft SQL Server 关系数据库解决方案，Microsoft Azure 云服务品牌等等。

硬件产品诸如鼠标、Xbox 游戏机、Zune 便携式媒体播放器、Kinect 肢体动作感应器等等。

### 创新研究

#### Microsoft HoloLens

Microsoft HoloLens 是 Windows Holographic 使用的主要设备。HoloLens 是一个 Windows 10 的智能眼镜产品。它采用先进的传感器、高清晰度 3D 光学头置式全角度透镜显示器以及环绕音效。它允许在增强现实中用户界面可以与用户透过眼神、语音和手势互相交流。其开发代号为“Project Baraboo”。HoloLens 由微软在 2015 年 1 月 21 日公布，在此之前已经开发了 5 年之久，其构想的一部分成为了在 2010 年发布的 Kinect。

#### Microsoft Band

Microsoft Band，是一款由微软开发与设计，带有健身手环功能的智能手表，并于 2014 年 10 月 29 日，在美国发售了限量版，售价 199 美元。Microsoft Band 于网上贩售时便已供不应求，在美国的大部分地区被抢购一空。

这个手表内含 10 个传感器，一个光学心率表，GPS 导航装置，和紫外线传感器，电池在充满电后能连续使用两天。这个装置的一部分功能依赖于它的配套 APP Microsoft Health，适用于 Windows Phone 8.1、Android 4.3+，和 iOS 7.1+。

它内建的应用程序称为“磁贴”（Tiles），例如有体能锻炼、紫外线、警铃与定时器、来电、简讯、日历、Facebook、气象等；如果搭配使用 Windows Phone 8.1 的装置，还能启用 Cortana。

#### Kinect

Kinect 是由微软开发，应用于 Xbox 360 和 Xbox One 主机的周边设备。它让玩家不需要手持或踩踏控制器，而是使用语音指令或手势来操作 Xbox 360 和 Xbox One 的系统界面。它也能捕捉玩家全身上下的动作，用身体来进行游戏，带给玩家“免控制器的游戏与娱乐体验”。此设备是微软研究院的研究成果之一。

Kinect 感应器是一个外型类似网络摄影机的装置。Kinect 有三个镜头，中间的镜头是 RGB 彩色摄影机，左右两边镜头则分别为红外线发射器和红外线 CMOS 摄影机所构成的 3D 结构光深度感应器。Kinect 还搭配了追焦技术，底座马达会随着对焦物体移动跟着转动。Kinect 也内建阵列式麦克风，由多组麦克风同时收音，比对后消除杂音。

## 三星

### 技术基础

和 Google 和 Apple 相比，我觉得三星的技术基础更能体现在硬件方面。例如主要的半导体业务，DRAM、NAND等闪存、微控制器以及影像感测器。近年来为了扩大市场，也进入了微处理器以及晶圆代工。

下一代 OLED 面板技术：AMOLED，它具有广色域、超广视角、高对比、自发光特性，无需背光模块及彩色滤光片、轻薄、反应速度快等优点，搭配小尺寸移动设备更是一股新兴潮流。

以及全套 3D 影音设备：包含了 3D 电视、3D 蓝光播放器、3D 影片和 3D 眼镜。

### 创新研究

#### Samsung Galaxy Gear

Samsung Galaxy Gear 是一款智能手表，由三星电子在 2013 年 9 月 4 日在柏林举行的发表会上公开。这款手表只能与 Galaxy Note 3 和 Galaxy Note 10.1 2014 Edition 搭配使用，Galaxy S4、Galaxy S III 和 Galaxy Note II 在升级到 Android 4.3 后也可搭配使用。

但是 Galaxy Gear 推出后受到不少批评，包括语音指令服务反应缓慢，转译错误，操作困难等等。

#### Samsung Gear 360

一体式全景相机，采用对称双鱼眼设计。

#### Samsung Gear IconX

Samsung Gear IconX，是韩国三星电子在 2016 年 7 月 15 日推出的蓝牙无线耳机。有三种颜色（黑色、蓝色和白色），内部存储可以足够容纳多达 1,000 个 MP3，通过蓝牙 4.1 协议链接至其他语音设备。

#### Samsung Gear VR

Samsung Gear VR 是一个移动的虚拟现实器件，由三星电子与 Oculus VR 公司合作开发。一个兼容的 Samsung Galaxy 器件 (Galaxy Note 5 或 Galaxy S6/S6 Edge)，分开销售，作为头戴式器件的显示器与处理器，而 Gear VR 单元本身包含高视野的透镜和定制的惯性测量单元（IMU），用于以 micro-USB 链接智能手机做旋转跟踪。相比用于 Google Cardboard 智能手机内部的惯性测量单元（IMU），这种 IMU 具有更优秀的准确性，以及更好的校准较低延迟。

三星 Gear VR 头戴式器件还包括在一侧的一个触摸板和回退按钮，以及一个接近传感器探测何时带上头戴式器件。触摸板和按键允许用户与虚拟环境交互的具有最低的标准输入能力，而 Google Cardboard 设备仅仅配备了一个按钮。

三星 Gear VR 最早是在 2014 年 9 月被宣布。为了允许开发人员创建 Gear VR 的内容，并允许 VR 技术爱好者尽早接触到技术，三星在发布消费者版本之前已经发布了 Gear VR 的两个创新者版本。

## Facebook

### 技术基础

Facebook 技术架构工程师 Steven Grimm 在博客中提及到：几乎我们所有的服务器都运行开源软件。我们的 Web 服务器是 Linux，Apache 和 PHP。我们数据库是 MySQL。我们使用 memcached 来保证网站的快速反应。一些后台应用 Python、Perl 和 Java，以及一些 gcc 和 Boost。程序员用 Subversion 和 git 来进行代码管理。还有很多——像很多网站一样，从头到脚都是开源软件。

### 创新研究

#### Facebook Portal

Facebook Portal 系列定位在主打支持视频通话的智能音箱。内置摄像头、麦克风，以及亚马逊 Alexa 语音助手，Portal 系列采用了自动追踪用户的移动摄像头，140 度广角，Smart Camera 功能可以根据用户的移动放大或缩小镜头，保证每个人都在视频画面中。Smart Sound 功能可以最大化消除背景杂音，突出人的声音，自动识别用户的距离，根据远近调节音量大小。用户可以通过 Portal 与 Messenger 上的联系人视频通话，而接收者只要拥有安装 Messenger 的手机或者平板即可。Portal 也将同时在线人数增加到七位。正如 Facebook Portal 的营销主管 Dave Kaufman 指出，Portal 不仅注重社交的广度，还注重社交的深度。为了做到让视频通话更加有趣，在其他视频功能上 Portal 添加了 AR 特效，Story Time 功能可以自定义声音和视觉效果，让使用者的脸模拟童话故事中的人物。

#### Oculus Rift

Oculus Rift 是一个虚拟现实头戴式显示器。

Oculus 的位置追纵系统，用来追纵用户头部与其他VR设备的位置，被称为“星座”（Constellation），具有一部可光学追纵 VR 设备的外部红外线摄影机。

Rift 或任何其他被此系统追纵的设备，都在其表面之上或之下配有一系统可精确定位的红外线 LED 灯，以特定模式闪烁。透过知道对象上 LED 灯的位置与其模式，系统可以亚毫米的准确度精确定位各设备的位置。

星座可使用单一个追纵摄影机或同时使用多个追纵摄影机。Rift 的包中已包含一个追纵摄影机，用户可单独购买其他摄影机，并放置在房间里，以获得更高的追纵容量，包括整个房间的容量。

## 亚马逊

### 技术基础

亚马逊以服务见长，2002 年，亚马逊公司推出亚马逊网路服务系统，为开发者的网站和客户端提供诸多云计算远端 Web 服务。2006 年 3 月，亚马逊简易储存服务上线，这是一项支持经由 HTTP 和 BitTorrent 协议将数据存储到服务器上的服务。

### 创新研究

#### Amazon Echo

Amazon Echo 是亚马逊公司所发售的一款搭载智能语音助理 Alexa 的智能音箱。Amazon Echo 共有三个版本，分别是入门级的 Dot、标准版的 Echo、以及携带版的 Tap，其中 Dot 的售价仅为 49.99 美元。

Echo 的外形和一般的蓝牙音箱没什么区别，也没有任何屏幕，唯一的交互方式就是语音。通过 Alexa 语音助手，用户通过简单的语音指令，就可以播放音乐、查询信息，甚至控制各种智能家居设备。

Echo 于 2014 年 11 月 6 日在亚马逊官网上线。2015 年，这款产品占据了整个音箱市场销量的 25%。2016 年，其销售量超过 650 万台。

#### Amazon Prime Air

Amazon Prime Air 是亚马逊目前正在开发的无人机交付服务。预计将从 2019 年底在部分城市开始运营。 该服务使用送货无人机在订购后 30 分钟内自动将单个包裹运送给客户。为了能够进行 30 分钟的交付，订单必须少于 5 磅（2.25 千克），足够小以适合将要携带的货箱，并且必须在 10 英里（16 英里）内有一个亚马逊的订单中心。

## 未来世界

### 仿生化

我认为未来的智能机器系统大致会朝两个方向发展，一个是和人越来越像，另一个是彻底摒弃和人相似的特征，转而模仿一些动物的外表。1970 年日本机器人专家森政弘曾提出过一个恐怖谷理论：由于机器人与人类在外表、动作上相似，所以人类亦会对机器人产生正面的情感；而当机器人与人类的相似程度达到一个特定程度的时候，人类对他们的反应便会突然变得极其负面和反感，哪怕机器人与人类只有一点点的差别，都会显得非常显眼刺目，从而整个机器人有非常僵硬恐怖的感觉，犹如面对行尸走肉；当机器人和人类的相似度继续上升，相当于普通人之间的相似度的时候，人类对他们的情感反应会再度回到正面，产生人类与人类之间的移情作用。在可以预见的将来，机器人的发展想要达到和人难以分辨的程度几乎是不可能的，所以无疑会陷入如上所说的“恐怖谷”中，而摆脱这一命运行之有效的方法之一是仿生学。

例如将智能机器设计成豹的外形，可以赋予其快速移动的能力；或者将其设计成蛇的外形，可以赋予其无视地形的能力；再或者设计成鸟的外形，赋予其飞行的能力。

利用仿生学一方面可以让智能机器拥有很多人类本身不具备的能力；另一方面是在可预见的将来避免智能机器陷入恐怖谷的一个有效方法。

### 触觉技术

触觉是人与外界环境直接接触时的重要感觉功能，研制满足要求的触觉传感器是机器人发展中的技术关键之一。随着微电子技术的发展和各种有机材料的出现，已经提出了多种多样的触觉传感器的研制方案，但目前大都属于实验室阶段，达到产品化的不多。触觉传感器按功能大致可分为接触觉传感器、力－力矩觉传感器、压觉传感器和滑觉传感器等。

赋予智能机器触觉是赋予它们智能必不可少的前提条件之一，有了触觉智能机器才会在此基础上进行实时反馈、精确的感知操控等等，才能实现更有效的自主。

### 多机器人协同

人类社会中合作的力量往往大于个人的力量，我想智能机器的世界中也应是如此。如今社会中的智能机器大多呈现出单体工作的模式，Google 的 Google Assistant 与其他智能家居的联动呈现出的智能机器的合作模式我想才是未来世界应有的样子。当然也不能被 Google 现有的成果限制了想象，而且这种协同还是类似于一主多从的模式，Google Assistant 作为高等智能机器操控者其他略微低等的智能家居。我觉得更合理的方式应该是每个智能机器都是对等的，它们可以自由地交流，高效地协作。但是赋予智能机器相互交流的能力似乎往往又会让人联想到智能生物反过来操控人类的情节桥段，恰如上一次讨论课所说的智能机器的本我自我超我的话题，我的观点是智能机器的行为是受到其本我指导的，而其本我又是由程序员即开发者设定好的。

### 人机交互

在第一点里提到，避免智能机器陷入恐怖谷的一个方法是寻求仿生学的帮助，而最根本的方法则是让其越过恐怖谷，也就是说让智能机器与人无法分辨，不管是外表上，还是情感上。这听上去有点像克隆人，但智能机器毕竟还是机器，没有克隆人那样的伦理问题。

而和人无法分辨的智能机器在和人的交流过程中也会不断地学习、不断地强化自己，使其能更好地服务于人。

## 技术障碍

### 仿生化

借助仿生学的帮助在现在来看我认为有两个技术难点：一是智能机器的骨骼系统，或者说有没有与其相适应的智能硬件与高性能材料。例如在高速移动的过程中，智能机器能不能承受一定程度的碰撞而依然能够正常工作；如果真的要达到无视地形的程度的话，其外表又能否真的像蛇那样光滑；再者，像鸟一样飞行的话，又要求其材质足够的轻巧。

二是与硬件和材料相匹配的软件系统。因为前者本身的操控和自由度毕竟还是有限的，只有开发与其相匹配的智能软件系统才能将仿生学的力量最大化。

### 触觉技术

触觉技术的话，我觉得最关键的一点是粒度要足够细。因为现有的一些智能机器已经是可以有一些触碰反馈的，但是和人相比，其精度是远远不足的。要达到精准的感知操控的话，至少要将智能机器的触觉反馈能力提升到人肉眼无法分辨的程度。

### 多机器人协同

目前市面上有的智能机器或是最新的一些研究成果，我认为对于智能机器的输入与输出之间的平衡很大程度上偏向前者。这也无可厚非，因为在现阶段智能机器接受人的指令才是较为关键的行为。而要建立一个多机器人协同的机制，需要解决智能机器如何有效输出，乃至如何输入别的智能机器而非人的输出的问题。有了这样的机制，才能达成多机器人协同的目标。

### 人机交互

这一点我觉得最难的地方就在于智能机器的内核，也就是 AI 系统上。因为现有的 AI 技术大多是针对某个特定领域，很少有普适性通用性很强的 AI。而人不一样，哪怕只是在很小的时候，人也具备了各种各样的能力以及喜怒哀乐的情感，想让智能机器拥有这样的能力，一方面要在一开始训练的时候有庞大的数据量的支撑，另一方面在 AI 系统内部，这些数据要有合理的组织形式才能模拟出人的样子，我觉得还有很长一段路要走。

## 参考文献

+ <https://zh.wikipedia.org/wiki/Android>
+ <https://www.zhihu.com/question/51280189>
+ <https://cloud.google.com/products/ai?hl=zh-cn>
+ <http://www.nsfc.gov.cn/csc/20340/20289/20760/index.html>
+ <https://zh.wikipedia.org/wiki/Android_Nougat>
+ <https://zh.wikipedia.org/wiki/Wear_OS>
+ <https://zh.wikipedia.org/wiki/Google%E5%8A%A9%E7%90%86>
+ <https://zh.wikipedia.org/wiki/Google_Allo>
+ <https://zh.wikipedia.org/wiki/Google_Home>
+ <https://zh.wikipedia.org/wiki/Google%E7%9F%A5%E8%AF%86%E5%9B%BE%E8%B0%B1>
+ <https://zh.wikipedia.org/wiki/Google_Lens>
+ <https://zh.wikipedia.org/wiki/Android_TV>
+ <https://zh.wikipedia.org/wiki/Google_Cast>
+ <https://zh.wikipedia.org/wiki/Siri>
+ <https://zh.wikipedia.org/wiki/AirPods>
+ <https://zh.wikipedia.org/wiki/HomePod>
+ <https://zh.wikipedia.org/wiki/Apple_Wallet>
+ <https://zh.wikipedia.org/wiki/Apple_TV>
+ <https://zh.wikipedia.org/wiki/Apple_Watch_(%E7%AC%AC%E4%B8%80%E4%BB%A3)>
+ <https://zh.wikipedia.org/wiki/%E5%BE%AE%E8%BD%AF>
+ <https://zh.wikipedia.org/wiki/Microsoft_HoloLens>
+ <https://zh.wikipedia.org/wiki/Microsoft_Band>
+ <https://zh.wikipedia.org/wiki/Kinect>
+ <https://zh.wikipedia.org/wiki/%E4%B8%89%E6%98%9F%E9%9B%BB%E5%AD%90>
+ <https://zh.wikipedia.org/wiki/%E4%B8%89%E6%98%9FGalaxy_Gear>
+ <https://zh.wikipedia.org/wiki/%E5%85%A8%E6%99%AF%E7%9B%B8%E6%9C%BA%E5%88%97%E8%A1%A8>
+ <https://zh.wikipedia.org/wiki/Samsung_Gear_IconX>
+ <https://zh.wikipedia.org/wiki/%E4%B8%89%E6%98%9FGear_VR>
+ <https://zh.wikipedia.org/wiki/Facebook>
+ <https://www.geekpark.net/news/233648>
+ <https://zh.wikipedia.org/wiki/Oculus_Rift>
+ <https://zh.wikipedia.org/wiki/%E4%BA%9E%E9%A6%AC%E9%81%9C%E5%85%AC%E5%8F%B8>
+ <https://zh.wikipedia.org/wiki/Amazon_Echo>
+ <https://en.wikipedia.org/wiki/Amazon_Prime_Air>
+ <https://baike.baidu.com/item/%E6%81%90%E6%80%96%E8%B0%B7%E7%90%86%E8%AE%BA>
+ <https://baike.baidu.com/item/%E8%A7%A6%E8%A7%89%E4%BC%A0%E6%84%9F%E5%99%A8/4206120>
+ <https://zhuanlan.zhihu.com/p/72684892>

## 问题

+ 单目摄像头与双目摄像头有什么区别？

  测距原理上，两种摄像头便完全不同。单目摄像头需要对目标进行识别，也就是说在测距前先识别障碍物是车、人还是别的什么。在此基础上再进行测距。而双目摄像头则更加像人类的双眼，主要通过两幅图像的视差计算来确定距离。也就是说，双目摄像头不需要知道障碍物是什么，只要通过计算就可以测距。