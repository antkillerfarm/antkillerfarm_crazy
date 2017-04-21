package oata;

import java.io.IOException;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.ansj.vec.Learn;
import com.ansj.vec.Word2VEC;
import com.hankcs.hanlp.HanLP;
import com.hankcs.hanlp.corpus.dependency.CoNll.CoNLLSentence;
import com.hankcs.hanlp.corpus.tag.Nature;
import com.hankcs.hanlp.seg.common.Term;
import joinery.DataFrame;
import org.apache.commons.lang3.StringUtils;

public class HelloWorld {

    public static void main(String[] args) {
        test7();
        //step2();
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
        //int num = 0;
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
            //num++;
            //if (num > 2000) {
            //	break;
            //}
        }
        FileWriter.put("/home/tj/big_data/data/talk/2j2.csv",corpus2);
    }
    public static boolean lineFilter(String line) {
        return (line.contains("系统提示:") || line.contains("系统提醒:"));
    }
    public static String formQuestionRegex() {
        List<String> questionKeywordList = Arrays.asList("\\?", "？","什么","几","吗","哪", "么");
        String regex = "(";
        for ( String keyword : questionKeywordList) {
            regex += keyword + "|";
        }
        regex = regex.substring(0,regex.length()-1);
        regex += ")";
        return regex;
    }
    public static boolean questionFilter(String line, String regex) {
        Pattern p = Pattern.compile(regex);
        Matcher m = p.matcher(line);
        return m.find();
    }
    public static void step3() {
        try {
            HanLP.Config.NNParserModelPath = "/home/tj/big_data/data/HanLP/" + HanLP.Config.NNParserModelPath;
            String regex = formQuestionRegex();
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2.csv", ",",null,false);
            DataFrame<Object> df2 = new DataFrame<>("keyword","class");
            int row_num = 0;
            double t1= 0, t2 = 0, t3 = 0;
            for (List<Object> row : df) {
                String talk = (String)row.get(1);
                List<String> keywordList = HanLP.extractKeyword(talk, 5);
                String keywordStr = StringUtils.join(keywordList, " ");
                //CoNLLSentence sentence = HanLP.parseDependency(talk);
                //int flag = QuestionParser.get().check(sentence.getWordArray(),talk);
                boolean flag = questionFilter(talk, regex);
                String talk_class;
                if (flag) {
                    talk_class = "Q";
                }
                else {
                    talk_class = "U";
                }
                df2.append(Arrays.asList(keywordStr, talk_class));

                row_num ++;
                if (row_num % 500 == 0) {
                    System.out.println(row_num);
                    //break;
                }
                //System.out.println("row");
            }
            DataFrame<Object> df3 = df.join(df2);
            df3.writeCsv("/home/tj/big_data/data/talk/2j3.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void test2() {
        try {
            String regex = formQuestionRegex();
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2.csv", ",",null,false);
            DataFrame<Object> df2 = new DataFrame<>("cutword");
            int row_num = 0;
            for (List<Object> row : df) {
                String talk = (String)row.get(1);
                List<Term> keywordList = HanLP.segment(talk);
                String keywordStr = "";
                for (Term term : keywordList) {
                    if (term.nature != Nature.w) {
                        keywordStr += term.word + " ";
                    }
                }

                df2.append(Arrays.asList(keywordStr));

                row_num ++;
                if (row_num % 500 == 0) {
                    System.out.println(row_num);
                    //break;
                }
                //System.out.println("row");
            }
            //DataFrame<Object> df3 = df.join(df2);
            df2.writeCsv("/home/tj/big_data/data/talk/2j3s.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void test3() {
        try {
            HanLP.Config.NNParserModelPath = "/home/tj/big_data/data/HanLP/" + HanLP.Config.NNParserModelPath;
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2.csv", ",",null,false);
            DataFrame<Object> df2 = new DataFrame<>("keyword","class");
            int row_num = 0;
            double t1= 0, t2 = 0, t3 = 0;
            for (List<Object> row : df) {
                String talk = (String)row.get(1);
                long startTime=System.nanoTime();
                List<String> keywordList = HanLP.extractKeyword(talk, 5);
                String keywordStr = StringUtils.join(keywordList, " ");
                long endTime= System.nanoTime();
                t1 += endTime - startTime;
                startTime = endTime;
                CoNLLSentence sentence = HanLP.parseDependency(talk);
                endTime=System.nanoTime();
                t2 += endTime - startTime;
                startTime = endTime;
                int flag = 0;//QuestionParser.get().check(sentence.getWordArray(),talk);
                endTime=System.nanoTime();
                t3 += endTime - startTime;
                String talk_class;
                if (flag ==1) {
                    talk_class = "Q";
                }
                else {
                    talk_class = "U";
                }
                df2.append(Arrays.asList(keywordStr, talk_class));

                row_num ++;
                if (row_num % 500 == 0) {
                    System.out.println(row_num);
                    //break;
                }
                //System.out.println("row");
            }
            System.out.println(String.format("%f:%f:%f", t1/1e9, t2/1e9, t3/1e9));
            DataFrame<Object> df3 = df.join(df2);
            df3.writeCsv("/home/tj/big_data/data/talk/2j3.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void test4() {
        try {
            DataFrame<Object> df = DataFrame.readCsv("/home/tj/big_data/data/talk/2j3.csv", ",", null, true);
            DataFrame<Object> df2 = df.select(new DataFrame.Predicate<Object>() {
                @Override
                public Boolean apply(List<Object> values) {
                    String str = (String)values.get(3);
                    return str.equals("Q");
                }
            });
            DataFrame<Object> df3 = df2.drop(0,2,3);
            df3.writeCsv("/home/tj/big_data/data/talk/2j4.csv");
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
        ss = "我的意思是，作为拖鞋是不是应该普通鞋大些？你让我按照普通鞋的尺寸来选，那是不是鞋本身比同码数的普通鞋大些？";
        regex = "(是不是|普通|尺寸)";
        p = Pattern.compile(regex);
        m = p.matcher(ss);
        System.out.println(m.find());
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
        content = "恩 那个手镯到了呢 因为发货的姐姐今天有急事要去处理 明天帮您发出呢 您看可以吗";
        sentence = HanLP.parseDependency(content);
        System.out.println(sentence);
        //System.out.println(QuestionParser.get().check(sentence.getWordArray(),content));
        System.out.println(HanLP.segment("你好，欢迎使用HanLP汉语处理包！"));
    }
    public static void test7() {
        try {
            Learn lean = new Learn();
            //lean.learnFile(new File("/home/tj/big_data/data/jinyong/utf8/x1.txt"));
            //lean.saveModel(new File("/home/tj/big_data/data/jinyong/utf8/vector.mod"));
            //lean.learnFile(new File("/home/tj/big_data/data/talk/2j3s.csv"));
            //lean.saveModel(new File("/home/tj/big_data/data/talk/vector.mod"));
            //加载测试
            Word2VEC w2v = new Word2VEC();

            w2v.loadJavaModel("/home/tj/big_data/data/talk/vector.mod");
            //w2v.loadGoogleModel("/home/tj/opensource/word2vec/vectors.bin");
            //System.out.println(w2v.distance("胡一刀"));
            float[] word_vector = w2v.getWordVector("顺丰");
            //System.out.println(word_vector.length);
            for(float value : word_vector) {
                System.out.println(value);
            }
            System.out.println();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
