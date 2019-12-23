package cse;

import org.apache.log4j.Logger;
import org.apache.log4j.Priority;
import org.apache.log4j.PropertyConfigurator;
public class App
{
    public static void main( String[] args )
    {
        args = new String[3];
        args[0] = "10.0.0.5";
        args[1] = "/etc/hosts";
        args[2] = "/etc/hosts";
        Executor.main(args);
    }
}
