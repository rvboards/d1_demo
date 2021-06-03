import java.io.PrintWriter;
import java.io.IOException;

public class Led {
	public Led(String devpath) throws IOException {
		this.devpath = devpath;
		setBrightness(0);
	}

	public void setBrightness(int brightness) throws IOException {
		PrintWriter pw = new PrintWriter(this.devpath);
		pw.printf("%d\n", brightness);
		pw.close();
	}

	private String devpath;
}
