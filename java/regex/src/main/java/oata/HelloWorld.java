package oata;

import java.io.FileReader;
import java.io.BufferedReader;
import java.lang.Exception;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.apache.commons.lang3.StringUtils;

public class HelloWorld {
    public static void main(String[] args) {
        //test1();
	test2();
    }
    public static void test1() {
	String str = "<input type='text' id='baijinshanQ' name='baijinshanW' >";
	String regEx="(?<=id=')[\\S\\s]+(?=' name)";
	Pattern p=Pattern.compile(regEx);
	Matcher m=p.matcher(str);
	boolean result=m.find();
	System.out.println(m.group(0));
    }
    public static void test2() {
	String line="";
	try{
	    FileReader fr=new FileReader("./1.txt");
	    BufferedReader br=new BufferedReader(fr);
	    line=br.readLine();
	
	    br.close();
	    fr.close();
	}catch(Exception e){}
	
	String regEx="(?<=\\\\\")[\\S\\s]+(?=\\\\\":)";
	Pattern p=Pattern.compile(regEx);
	Matcher m=p.matcher(line);
	boolean result=m.find();
	System.out.println(m.group());

        regEx="\\\\\"[\\S\\s]+\\\\\":";
        p=Pattern.compile(regEx);
        m=p.matcher(line);
        result=m.find();
	System.out.println(m.group());
	System.out.println(m.group().replaceAll("\\\\\"","\""));
	System.out.println(StringUtils.replace(m.group(),"\\\"","\""));
    }
}
