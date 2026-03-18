package kr.co.songs.android.AlienInvasion.http;

public class CThreadEvent {
	private final Object lock = new Object();
	static int INFINITE = 99999999;
	
	
	public void signalAll() {
        synchronized (lock) {
            lock.notifyAll();
        }
    }

    public void signal() {
        synchronized (lock) {
            lock.notify();
        }
    }

    public void await(long timeout) throws InterruptedException {
        synchronized (lock) {
        	if(timeout == INFINITE)
        		lock.wait();
        	else
        		lock.wait(timeout);
        	
        }
    }
    
    public void await() throws InterruptedException {
        await(INFINITE);
    }
}
