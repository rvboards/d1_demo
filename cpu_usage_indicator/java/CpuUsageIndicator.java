import java.io.IOException;
import java.lang.Thread;
import sun.misc.SignalHandler;
import sun.misc.Signal;


public class CpuUsageIndicator {
	public static void main(String[] args) throws IOException, InterruptedException {
		App app = new App();

		app.run();
	}
}

class App {
	public App() throws IOException{
		stat = new ProcStat();
		ledr = new Led("/sys/class/leds/sunxi_led0r/brightness");
		ledg = new Led("/sys/class/leds/sunxi_led0g/brightness");
		running = true;

		SignalHandler handler = new SignalHandler() { 
			public void handle(Signal sig) { 
				running = false;
			} 
		}; 
		Signal.handle(new Signal("INT"),handler); 
	}

	public void run() throws IOException, InterruptedException{
		while(running) {
			Thread.sleep(800);

			ledr.setBrightness(0);
			ledg.setBrightness(0);

			Thread.sleep(200);

			usage = stat.getCpuUsage();
			System.out.printf("\rcpu usage = %d %%  ", usage);
			System.out.flush();

			if(usage < 30) {
				ledg.setBrightness(30);
			} else if(usage < 60) {
				ledg.setBrightness(5);
			} else if(usage < 90) {
				ledr.setBrightness(15);
			} else {
				ledr.setBrightness(150);
			}
		}

		System.out.printf("\n");
		ledr.setBrightness(0);
		ledg.setBrightness(0);
	}

	private class ExitHandler extends Thread {
		public ExitHandler() {
			super("Exit Handler");
		}

		public void run() {
			running = false;
		}
	}

	public boolean running;

	int usage;
	ProcStat stat;
	Led ledr;
	Led ledg;
}
