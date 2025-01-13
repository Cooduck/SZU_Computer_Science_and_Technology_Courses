package test4;

public class StopWatch {
    private long startTime; // 记录开始时间
    private long endTime;   // 记录结束时间

    // 开始计时
    public void start() {
        startTime = System.currentTimeMillis(); // 获取当前时间
    }

    // 停止计时
    public void stop() {
        endTime = System.currentTimeMillis(); // 获取当前时间
    }

    // 返回所花费的时间（单位：秒）
    public double getRunningTime() {
        return (endTime - startTime) / 1000.0; // 计算并返回时间差
    }
}
