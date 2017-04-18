package oata;

import com.hankcs.hanlp.HanLP;
import joinery.DataFrame;

import java.io.IOException;


public class HelloWorld {
    public static void main(String[] args) {
        test1();
        test2();
    }
    public static void test1() {
        System.out.println(HanLP.segment("你好，欢迎使用HanLP汉语处理包！"));
    }
    public static void test2() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/c2c.csv", ",",null,false);
            System.out.println(df.count());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
