DbgInfo.m——如何处理大尺寸的Dbginfo文件，主要功能点包括大文件的分段读取和查找字符串。
DbgInfo文件夹
    BatDbgHandler.m和DirHandler.m包含了如何采用递归的办法，遍历文件夹（包括所有的子文件夹）中的所有Dbginfo文件。
    BuHandler.m用于处理使用FPGA比较多路PPS时间差异的输出文件。
    BuCfgHandler.m用于处理Dbginfo中的BuCfg数据。
    FileHandler.m用于检查是否存在导航数据不连续的问题。主要用于处理TD3005系列硬件带来的信号不稳定的问题。
    FileHandler1.m用于处理GGA语句的输出时间。


