package oata;

import java.io.IOException;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.hankcs.hanlp.HanLP;
import com.hankcs.hanlp.corpus.dependency.CoNll.CoNLLSentence;
import joinery.DataFrame;
import org.apache.commons.lang3.StringUtils;

public class HelloWorld {

    public static void main(String[] args) {
        //test6();
        step3();
        //System.out.println("row");
    }
    public static void step1() {
        List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2.txt", "txt");
        List<String> corpus2 =  new LinkedList<>();
        for(String sent : corpus){
            sent = sent.replaceAll(",", "，");
            sent = sent.replaceAll("\"", "”");
            String regex = "(http|ftp|https)://\\S*";
            Pattern p = Pattern.compile(regex);
            Matcher m = p.matcher(sent);
            sent = m.replaceAll("，TAGURL，");
            regex = "/:[^/]{2,4}";
            p = Pattern.compile(regex);
            m = p.matcher(sent);
            sent = m.replaceAll("，TAGEMOJI，");
            corpus2.add(sent);
        }
        FileWriter.put("/home/tj/big_data/data/talk/2j.txt",corpus2);
    }
    public static void step2() {
        FileReader.setFile_charset("UTF-8");
        List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2j.txt", "txt");
        if (corpus == null) return;
        List<String> corpus2 =  new LinkedList<>();
        boolean start_flag = false;
        boolean first_flag = false;
        String s_name = null;

        for(String sent : corpus){
            if (!first_flag) {
                int pos = sent.indexOf(":");
                s_name = sent.substring(0,pos);
                System.out.println(s_name);
                first_flag = true;
                continue;
            }
            String regex1 = "^[0-9]{4}-[0-9]{2}-[0-9]{2}";
            Pattern p1 = Pattern.compile(regex1);
            Matcher m1 = p1.matcher(sent);
            String regex2 = "^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}";
            Pattern p2 = Pattern.compile(regex2);
            Matcher m2 = p2.matcher(sent);
            boolean sent_flag = m2.find();
            if (!start_flag && m1.find() && !sent_flag) {
                start_flag = true;
                continue;
            }
            if (sent_flag) {
                int pos1 = StringUtils.ordinalIndexOf(sent, " ", 2);
                int pos2 = StringUtils.ordinalIndexOf(sent, " ", 3);
                if (pos2 == -1)
                {
                    continue;
                }
                String name = sent.substring(pos1+1,pos2-1);
                String talk = sent.substring(pos2+1);
                String str;
                if (name.contains(s_name)) {
                    str = "S,";
                } else {
                    str = "C,";
                }
                if (start_flag) {
                    str = str + "TAGSTART，";
                    start_flag = false;
                }
                str = str + talk;
                boolean flag2 = lineFilter(talk);
                if (!flag2 && talk.length() > 0) {
                    corpus2.add(str);
                }
            }
        }
        FileWriter.put("/home/tj/big_data/data/talk/2j2.csv",corpus2);
    }
    public static boolean lineFilter(String line) {
        return (line.contains("系统提示:") || line.contains("系统提醒:"));
    }
    public static void step3() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2.csv", ",",null,false);
            DataFrame<Object> df2 = new DataFrame<>("keyword");
            int row_num = 0;
            for (List<Object> row : df) {
                String talk = (String)row.get(1);
                List<String> keywordList = HanLP.extractKeyword(talk, 5);
                String keywordStr = StringUtils.join(keywordList, " ");
                df2.append(Arrays.asList(keywordStr));

                row_num ++;
                if (row_num % 500 == 0) {
                    System.out.println(row_num);
                }
                //System.out.println("row");
            }

            DataFrame<Object> df3 = df.join(df2);
            df3.writeCsv("/home/tj/big_data/data/talk/2j3.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void test5() {
        String regex = "(http|ftp|https)://\\S*";
        Pattern p = Pattern.compile(regex);
        String ss = "2016-10-12 15:21:40 wzy9008: http://interface.im.taobao.com/mobileimweb/fileupload";
        Matcher m = p.matcher(ss);
        ss = m.replaceAll("TAGURL");
        System.out.println(ss);
        int pos1 = StringUtils.ordinalIndexOf(ss, " ", 2);
        int pos2 = StringUtils.ordinalIndexOf(ss, " ", 3);
        String str = String.format("%d：%d", pos1, pos2);
        String str2 = ss.substring(pos1+1,pos2-1);
        System.out.println(str);
        System.out.println(str2);
    }
    public static void test6() {
        HanLP.Config.NNParserModelPath = "/home/tj/big_data/data/HanLP/" + HanLP.Config.NNParserModelPath;
        String content = "程序员(英文Programmer)是从事程序开发、维护的专业人员。一般将程序员分为程序设计人员和程序编码人员，但两者的界限并不非常清楚，特别是在中国。软件从业人员分为初级程序员、高级程序员、系统分析员和项目经理四大类。";
        CoNLLSentence sentence = HanLP.parseDependency(content);
        System.out.println(sentence);
        List<String> keywordList = HanLP.extractKeyword(content, 5);
        System.out.println(keywordList);
        List<String> sentenceList = HanLP.extractSummary(content, 3);
        System.out.println(sentenceList);
    }
}
