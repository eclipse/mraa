
public class Isr {
	static {
    		try {
			System.loadLibrary("mraajava");
		    } catch (UnsatisfiedLinkError e) {
	      System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
   	   System.exit(1);
    	}
  	}
	public static void main(String argv[]) {
		mraa.mraa.init();

		mraa.Gpio gpio = new mraa.Gpio(7);


		mraa.IsrCallback callback = new JavaCallback();

    		gpio.isr(mraa.Edge.EDGE_RISING, callback, null);
		while(true);
	};
};

class JavaCallback extends mraa.IsrCallback
{
  public JavaCallback()
  {
    super();
  }

  public void run()
  {
    System.out.println("JavaCallback.run()");
  }
}

