import java.util.Scanner;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

public class ProcStat {
	public ProcStat() throws FileNotFoundException, IOException {
		read();
		calc();
	}
	
	protected void read() throws FileNotFoundException, IOException {
		InputStream f = new FileInputStream("/proc/stat");
		Scanner scan = new Scanner(f);

		String str = scan.next();
		this.user = Integer.parseInt(scan.next());
		this.system = Integer.parseInt(scan.next());
		this.nice = Integer.parseInt(scan.next());
		this.idle = Integer.parseInt(scan.next());
		this.iowait = Integer.parseInt(scan.next());
		this.irq = Integer.parseInt(scan.next());
		this.softirq = Integer.parseInt(scan.next());

		f.close();
	}

	protected void calc() {
		this.total = this.user;
		this.total += this.nice;
		this.total += this.system;
		this.total += this.idle;
		this.total += this.iowait;
		this.total += this.irq;
		this.total += this.softirq;

		this.used = this.total - this.idle;
	}

	public int getCpuUsage() throws FileNotFoundException, IOException {
		long usage;
		ProcStat stat_latest = new ProcStat();

		usage = (stat_latest.used - this.used) * 100 / (stat_latest.total - this.total);

		this.used = stat_latest.used;
		this.total = stat_latest.total;

		return (int)usage;
	}

	private long user;
	private long system;
	private long nice;
	private long idle;
	private long iowait;
	private long irq;
	private long softirq;

	public long total;
	public long used;
}

