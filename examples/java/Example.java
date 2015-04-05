
public class Example {
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
		System.out.println(mraa.mraa.getVersion());
	};
};

