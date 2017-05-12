package oata;

import com.ansj.vec.Learn;
import com.ansj.vec.Word2VEC;
import com.ansj.vec.util.MapCount;
import com.hankcs.hanlp.HanLP;
import com.hankcs.hanlp.corpus.dependency.CoNll.CoNLLSentence;
import com.hankcs.hanlp.corpus.tag.Nature;
import com.hankcs.hanlp.seg.common.Term;
import info.debatty.java.stringsimilarity.Jaccard;
import joinery.DataFrame;
import org.apache.commons.lang3.StringUtils;
import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;
import weka.filters.Filter;
import weka.filters.unsupervised.attribute.Remove;

import java.io.File;
import java.io.IOException;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class App2 {
    public static void main(String[] args) {
        //App.step1_2();
        App2 app = new App2();
        app.import_flowerplus_1();
        //App.step1();
        //step2();
        //step3();
        //step4();
        //step5_2_4();
        //step6_1();
        //step7();
        //test7();
        //step2_2();
        //test_dbscan();
    }
    public static void step1() {
        List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2.txt", "txt");
        int dlg_num = 0;
        if (corpus == null) return;

        for(String sent : corpus){
            String regex1 = "^[0-9]{4}-[0-9]{2}-[0-9]{2}";
            Pattern p1 = Pattern.compile(regex1);
            Matcher m1 = p1.matcher(sent);
            String regex2 = "^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}";
            Pattern p2 = Pattern.compile(regex2);
            Matcher m2 = p2.matcher(sent);
            boolean sent_flag = m2.find();
            if (m1.find() && !sent_flag) {
                dlg_num++;
                if (dlg_num % 100 == 0) {
                    System.out.println(dlg_num);
                }
            }
        }
        System.out.println(dlg_num);
    }
    public static void step2() {
        FileReader.setFile_charset("UTF-8");
        List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2jx.csv", "txt");
        if (corpus == null) return;
        List<String> corpus2 =  new LinkedList<>();
        boolean start_flag = false;
        boolean first_flag = false;
        String s_name = null;
        int num = 0, start_num = 0, end_num = 0;
        String str = null;
        for(String sent : corpus){
            num++;
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
                end_num = num;
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

                if (start_flag) {
                    if (str != null) {
                        corpus2.add(String.format("%d,%d,%s", start_num, end_num, str));
                    }
                    start_num = num;
                    str = "";
                    start_flag = false;
                }
                str = str + talk + "；";
            }
        }
        corpus2.add(String.format("%d,%d,%s", start_num, num, str));
        FileWriter.put("/home/tj/big_data/data/talk/2j2c.csv",corpus2);
    }
    public static void step3() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2c.csv", ",",null,false);
            DataFrame<Object> df2 = new DataFrame<>();
            int row_num = 0;
            for (List<Object> row : df) {
                Long start_num = (Long)row.get(0);
                Long end_num = (Long)row.get(1);
                String talk = (String)row.get(2);
                talk = talk.replaceAll("；", "");
                List<Term> keywordList = HanLP.segment(talk);
                String keywordStr = "";
                for (Term term : keywordList) {
                    if (term.nature != Nature.w) {
                        keywordStr += term.word + " ";
                    }
                }

                df2.append(Arrays.asList(start_num, end_num, keywordStr));

                row_num ++;
                if (row_num % 500 == 0) {
                    System.out.println(row_num);
                    //break;
                }
                //System.out.println("row");
            }
            //DataFrame<Object> df3 = df.join(df2);
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step4() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc.csv", ",",null,true);
            DataFrame<Object> df2 = new DataFrame<>("keyword");
            int row_num = 0;
            for (List<Object> row : df) {
                String talk = (String)row.get(2);
                if (talk==null)
                {
                    talk = "";
                }
                List<String> keywordList = HanLP.extractKeyword(talk, 20);
                String[] talk_kw_list = keywordList.toArray(new String[keywordList.size()]);
                Arrays.sort(talk_kw_list);
                String keywordStr = "";
                for (String kw : talk_kw_list) {
                    if (kw.equals("TAGEMOJI") || kw.equals("TAGURL")) {
                        continue;
                    }
                    if (!kw.equals("TAGSTART")) {
                        if (keywordStr.equals("")) {
                            keywordStr += kw;
                        }else {
                            keywordStr += " " + kw;
                        }
                    }
                }
                //String keywordStr = StringUtils.join(keywordList, " ");
                df2.append(Arrays.asList(keywordStr));

                row_num ++;
                if (row_num % 500 == 0) {
                    System.out.println(row_num);
                    //break;
                }
                //System.out.println("row");
            }
            DataFrame<Object> df3 = df.join(df2);
            //DataFrame<Object> df4 = df3.groupBy(2).count().sortBy(-1);
            df3.writeCsv("/home/tj/big_data/data/talk/2j3sc_2.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step5() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_2.csv", ",",null,true);
            DataFrame<Object> df2 = df.groupBy(2).count().sortBy(-1);
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_3.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step5_1() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_2.csv", ",",null,true);
            DataFrame<Object> df2 = new DataFrame<>();
            for (List<Object> row : df) {
                Long start_num = (Long)row.get(0);
                Long end_num = (Long)row.get(1);
                String talk = (String)row.get(2);
                String key = (String)row.get(3);
                if (key != null && key.contains("退款")) {
                    df2.append(Arrays.asList(start_num, end_num, talk, key));
                }
            }
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_4.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step5_2() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_2.csv", ",",null,true);
            DataFrame<Object> df2 = new DataFrame<>();
            List<Nature> natureList = Arrays.asList(Nature.vn);
            for (List<Object> row : df) {
                Long start_num = (Long)row.get(0);
                Long end_num = (Long)row.get(1);
                String talk = (String)row.get(2);
                String key = (String)row.get(3);
                if (key == null) {
                    continue;
                }
                List<Term> term_list = HanLP.segment(key);
                String vn_key = "";
                for (Term term : term_list) {
                    if (natureList.contains(term.nature)) {
                        vn_key += term.word + " ";
                    }
                }
                df2.append(Arrays.asList(start_num, end_num, talk, key, vn_key));
            }
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_4_1.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step5_2_2() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4_1.csv", ",",null,true);
            DataFrame<Object> df2 = df.groupBy(4).count().sortBy(-1);
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_4_2.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step5_2_3() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4_1.csv", ",",null,true);
            DataFrame<Object> df2 = DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_6.csv", ",",null,true);
            DataFrame<Object> df3 = new DataFrame<>();
            DataFrame<Object> df5 = new DataFrame<>();
            HashMap<String, String> talk_class = new HashMap<>();
            int df_len = df.length();
            int df2_len = df2.length();
            for (int i = 0; i < df2_len - 1; i++) {
                for (int j = i + 1; j < df2_len; j++) {
                    String keyword0 = (String)df2.get(i,0);
                    String keyword1 = (String)df2.get(j,0);
                    List<String> kw_list = Arrays.asList(keyword0, keyword1);
                    List<String> corpus =  new LinkedList<>();
                    DataFrame<Object> df4 = new DataFrame<>();
                    for (int k = 0; k < df_len; k++) {
                        String vn_key = (String)df.get(k,4);
                        if (vn_key == null) {
                            continue;
                        }
                        int match_flag = 0;
                        for (String kw : kw_list) {
                            if (vn_key.contains(kw)) {
                                match_flag++;
                            }
                        }
                        if (match_flag == kw_list.size()) {
                            Long start_num = (Long)df.get(k,0);
                            Long end_num = (Long)df.get(k,1);
                            String talk = (String)df.get(k,2);
                            String talk_keyword = (String)df.get(k,3);
                            String keywordStr = StringUtils.join(kw_list, " ");
                            if (!talk_class.containsKey(String.valueOf(k))) {
                                talk_class.put(String.valueOf(k),keywordStr);
                                df4.append(Arrays.asList(start_num, end_num, talk, talk_keyword, vn_key, keywordStr));
                                corpus.add(talk_keyword);
                            }
                        }
                    }
                    int clunum = 0;
                    if (corpus.size() != 0) {
                        DBScan ds = new DBScan();
                        ArrayList<DBScan.DataObject> source = ds.formDataObject(corpus);
                        clunum = ds.dbscan(source);
                        int df4_len = df4.length();
                        HashMap<String, String> cid_class = new HashMap<>();
                        int source_num = 0;
                        for (int n = 0; n < df4_len; n++) {
                            int cid = source.get(n).getCid();
                            boolean cid_flag = false;
                            if (cid == -1) {
                                cid_flag = true;
                            } else
                            {
                                if (!cid_class.containsKey(String.valueOf(cid))) {
                                    cid_class.put(String.valueOf(cid), "");
                                    cid_flag = true;
                                }
                            }
                            if (cid_flag) {
                                Long start_num = (Long) df4.get(n, 0);
                                Long end_num = (Long) df4.get(n, 1);
                                String talk = (String) df4.get(n, 2);
                                String talk_keyword = (String) df4.get(n, 3);
                                String vn_key = (String) df4.get(n, 4);
                                String keywordStr = (String) df4.get(n, 5);
                                df3.append(Arrays.asList(start_num, end_num, talk, talk_keyword, vn_key, keywordStr, cid));
                                source_num++;
                            }
                        }
                        String keywordStr = (String) df4.get(0, 5);
                        df5.append(Arrays.asList(keywordStr, source_num));
                    }

                    System.out.println(String.format("%d:%d:%d:%d", i, j, clunum, corpus.size()));
                }
            }

            df3.writeCsv("/home/tj/big_data/data/talk/2j3sc_4_3.csv");
            df5.writeCsv("/home/tj/big_data/data/talk/2j3sc_4_4.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step5_2_4() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4_3.csv", ",",null,true);
            DataFrame<Object> df2 =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4_4.csv", ",",null,true);
            FileReader.setFile_charset("UTF-8");
            List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2jx.csv", "txt");
            List<String> corpus2 = new LinkedList<>();
            String cur_key = null;
            String s = null;
            for (List<Object> row : df) {
                String keywordStr = (String)row.get(5);
                if (!keywordStr.equals(cur_key)) {
                    for (List<Object> row2 : df2) {
                        String keywordStr2 = (String)row2.get(0);
                        if (keywordStr2.equals(keywordStr)) {
                            Long num = (Long)row2.get(1);
                            s = "--------------------\n";
                            s += String.format("%s:%d\n", keywordStr2, num);
                            s += "--------------------\n";
                            corpus2.add(s);
                            break;
                        }
                    }
                }
                cur_key = keywordStr;
                Long start_num = (Long)row.get(0);
                Long end_num = (Long)row.get(1);
                for (Long i = start_num - 1; i < end_num - 1; i++) {
                    corpus2.add(corpus.get(i.intValue()));
                }
                s = "--------------------\n";
                corpus2.add(s);
            }
            FileWriter.put("/home/tj/big_data/data/talk/2j3sc_4_5.txt",corpus2);
            System.out.println("bye");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step6() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4.csv", ",",null,true);
            FileReader.setFile_charset("UTF-8");
            List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2jx.csv", "txt");
            List<String> corpus2 = new LinkedList<>();
            String s = "--------------------\n";
            s += String.format("退款:%d\n", df.length());
            s += "--------------------\n";
            corpus2.add(s);
            for (List<Object> row : df) {
                Long start_num = (Long)row.get(0);
                Long end_num = (Long)row.get(1);
                for (Long i = start_num - 1; i < end_num - 1; i++) {
                    corpus2.add(corpus.get(i.intValue()));
                }
                s = "--------------------\n";
                corpus2.add(s);
            }
            FileWriter.put("/home/tj/big_data/data/talk/2j3sc_5.txt",corpus2);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step6_1() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4.csv", ",",null,true);
            MapCount kw_count_map = new MapCount();
            for (List<Object> row : df) {
                String talk_keyword = (String)row.get(3);
                String[] talk_kw_list = StringUtils.split(talk_keyword, " ");
                if (talk_kw_list != null) {
                    kw_count_map.add(talk_kw_list);
                }
            }
            List<Map.Entry<String, Integer>> kw_count_list = new ArrayList<Map.Entry<String, Integer>>(kw_count_map.get().entrySet());
            Collections.sort(kw_count_list, new Comparator<Map.Entry<String, Integer>>() {
                public int compare(Map.Entry<String, Integer> o1, Map.Entry<String, Integer> o2) {
                    return (o2.getValue() - o1.getValue());
                    //return (o1.getKey()).toString().compareTo(o2.getKey());
                }
            });
            DataFrame<Object> df2 = new DataFrame<>("kw", "count");
            for (int i = 0; i < kw_count_list.size(); i++) {
                String key = kw_count_list.get(i).getKey();
                List<Term> term_list = HanLP.segment(key);
                for (Term term : term_list) {
                    //System.out.println(term.word + "/" + term.nature);
                    //if (natureList.contains(term.nature)) {
                    df2.append(Arrays.asList(key + "/" + term.nature, kw_count_list.get(i).getValue()));
                    //total_count += kw_count_list.get(i).getValue();
                }
            }
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_6.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step6_2() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_6.csv", ",",null,true);
            DataFrame<Object> df2 = new DataFrame<>("kw", "count", "cdf");
            int num = 0;
            for (List<Object> row : df) {
                String talk_keyword = (String)row.get(0);
                if (talk_keyword.contains("退款")) {
                    continue;
                }
                Long talk_keyword_cnt = (Long)row.get(1);
                if (talk_keyword.contains("vn")) {
                    num += talk_keyword_cnt;
                    df2.append(Arrays.asList(talk_keyword, talk_keyword_cnt, num));
                }
            }
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_6_2.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step7() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_4_1.csv", ",",null,true);
            DataFrame<Object> df2 = new DataFrame<>();
            String[] kw_list = {"邮发","活动"};
            for (List<Object> row : df) {
                String talk_keyword = (String)row.get(3);
                //String[] talk_kw_list = StringUtils.split(talk_keyword, " ");
                int match_flag = 0;
                for (String kw : kw_list) {
                    if (talk_keyword.contains(kw)) {
                        match_flag++;
                    }
                }
                if (match_flag == kw_list.length) {
                    Long start_num = (Long)row.get(0);
                    Long end_num = (Long)row.get(1);
                    String talk = (String)row.get(2);
                    String vn_key = (String)row.get(4);
                    df2.append(Arrays.asList(start_num, end_num, talk, talk_keyword, vn_key));
                }
            }

            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_7.csv");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void step2_2() {
        FileReader.setFile_charset("UTF-8");
        List<String> sources = FileReader.getAll("/home/tj/big_data/data/talk/2j.txt", "txt");
        List<String> lines = FileReader.getAll("/home/tj/big_data/data/talk/2j3_7_4.txt", "txt");
        List<String> common = new LinkedList<>();
        List<String> corpus = new LinkedList<>();
        int start_flag = 0;
        for (String line : lines) {
            if (line.contains("----------")) {
                start_flag++;
                if (start_flag == 3) {
                    break;
                }
                continue;
            }
            if (start_flag == 2) {
                line = line.replaceAll("TAGSTART，", "");
                common.add(line);
            }
        }
            /*for (List<Object> row : df) {
                String role = (String) row.get(0);
                String talk = (String) row.get(1);
                if (!common.contains(talk)) {
                    df2.append(Arrays.asList(role, talk));
                }
            }
            df2.writeCsv("/home/tj/big_data/data/talk/2j3sc_3.csv");*/
        for (String source : sources) {
            int flag = 0;
            for (String talk : common) {
                if (source.contains(talk)) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
            {
                corpus.add(source);
            }
        }
        FileWriter.put("/home/tj/big_data/data/talk/2jx.csv",corpus);
    }
    public static void test_reg() {
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
    public static void test_nlp() {
        HanLP.Config.NNParserModelPath = "/home/tj/big_data/data/HanLP/" + HanLP.Config.NNParserModelPath;
        String content = "程序员(英文Programmer)是从事程序开发、维护的专业人员。一般将程序员分为程序设计人员和程序编码人员，但两者的界限并不非常清楚，特别是在中国。软件从业人员分为初级程序员、高级程序员、系统分析员和项目经理四大类。";
        CoNLLSentence sentence = HanLP.parseDependency(content);
        System.out.println(sentence);
        List<String> keywordList = HanLP.extractKeyword(content, 5);
        System.out.println(keywordList);
        List<String> sentenceList = HanLP.extractSummary(content, 3);
        System.out.println(sentenceList);
        content = "恩 那个手镯到了呢 因为发货的姐姐今天有急事要去处理 明天帮您发出呢 您看可以吗?都是澳洲直邮的";
        sentence = HanLP.parseDependency(content);
        System.out.println(sentence);
        //System.out.println(QuestionParser.get().check(sentence.getWordArray(),content));
        List<Term> term_list = HanLP.segment(content);
        for (Term term : term_list) {
            System.out.println(term.word + "/" + term.nature);
        }
        System.out.println("ok");
        List<Nature> natureList = Arrays.asList(Nature.v, Nature.vn);
        for (Term term : term_list) {
            if (natureList.contains(term.nature)) {
                System.out.println(term.word + "/" + term.nature);
            }
        }
    }
    public static void test_wordvec() {
        try {
            Learn lean = new Learn();
            //lean.learnFile(new File("/home/tj/big_data/data/jinyong/utf8/x1.txt"));
            //lean.saveModel(new File("/home/tj/big_data/data/jinyong/utf8/vector.mod"));
            lean.learnFile(new File("/home/tj/big_data/data/talk/2j3s.csv"));
            lean.saveModel(new File("/home/tj/big_data/data/talk/vector.mod"));
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
    public static void test_wordvec_2() {
        try {
            Word2VEC w2v = new Word2VEC();
            w2v.loadGoogleModel("/home/tj/big_data/data/word2vec_weixin/word2vec_c.bin");
            System.out.println(w2v.distance("金庸"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void test_jaccard() {
        System.out.println("Jaccard");
        Jaccard j2 = new Jaccard(2);
        // AB BC CD DE DF
        // 1  1  1  1  0
        // 1  1  1  0  1
        // => 3 / 5 = 0.6
        System.out.println(j2.similarity("ABCDE", "ABCDF"));
        System.out.println(j2.similarity("s1 must not be null", "s2 must not be null"));
    }
    public static void test_jaccard_2() {
        System.out.println("Jaccard");
        System.out.println(DBScan.calJaccardDist("s1 must not be null", "s2 must not be null"));
    }
    public static void test_weka() {
        try {
            DataSource source = new DataSource("/home/tj/big_data/data/talk/2j3sc_4.csv");
            Instances inst;
            inst = source.getDataSet();
            int[] indicesOfColumnsToUse = {3};
            Remove remove = new Remove();
            remove.setAttributeIndicesArray(indicesOfColumnsToUse);
            remove.setInvertSelection(true);
            remove.setInputFormat(inst);

            Instances instSubset = Filter.useFilter(inst, remove);
            //Instance i1 = i.get(1);
            System.out.println(instSubset);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void test_dbscan() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3sc_7.csv", ",",null,true);
            List<String> corpus =  new LinkedList<>();
            for (List<Object> row : df) {
                String talk_keyword = (String)row.get(3);
                corpus.add(talk_keyword);
            }
            System.out.println("step1");
            DBScan ds=new DBScan();
            ArrayList<DBScan.DataObject> source = ds.formDataObject(corpus);
            int clunum=ds.dbscan(source);
            System.out.println("step2");
            int num = 0;
            for (DBScan.DataObject object : source) {
                if (object.getCid() == -1) {
                    System.out.println(object.getValue());
                    num++;
                }
            }
            System.out.println(String.format("%d:%d:%d", clunum, corpus.size(), num));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static boolean isIDValid(String role) {
        if (role.startsWith("wx") || role.startsWith("TJ_IM_WEB_WX_USER_ACCOUNT_")) {
            return true;
        }
        return false;
    }
    public static String formKey(String s1, String s2) {
        String customer, service;
        if (s1.startsWith("wx")) {
            return s1+"/"+s2;
        }
        else {
            return s2+"/"+s1;
        }
    }
    public static void import_flowerplus() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/3j.csv", ",",null,false);
            DataFrame<Object> df2 = df.sortBy(1);
            df2.writeCsv("/home/tj/big_data/data/talk/3j_2.csv");
            System.out.println("bye");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void import_flowerplus_1() {
        try {
            DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/3j.csv", ",",null,false);
            Map<String,List<Talk>> talks_map = new HashMap<>();
            List<String> corpus2 =  new LinkedList<>();
            for (List<Object> row : df) {
                String session_id = (String)row.get(1);
                String from = (String)row.get(3);
                String to = (String)row.get(5);
                if (isIDValid(from) && isIDValid(to)) {
                    String key = session_id;//formKey(from, to);
                    List<Talk> talks;
                    if (!talks_map.containsKey(key)) {
                        talks = new LinkedList<>();
                        talks_map.put(key,talks);
                    } else {
                        talks = talks_map.get(key);
                    }
                    String s = (String)row.get(6);
                    if (s != null) {
                        Talk talk = new Talk();
                        talk.from = from;
                        talk.to = to;
                        talk.talk = lineFilter(s);
                        talks.add(talk);
                    }
                }
            }
            int num = 0;
            for (List<Talk> talks: talks_map.values()) {
                for (Talk talk: talks) {
                    String role;
                    if (talk.from.startsWith("wx")) {
                        role = "C";
                    }else {
                        role = "S";
                    }
                    String s = String.format("%s:%s", role, talk.talk);
                    corpus2.add(s);
                    num++;
                }
                corpus2.add("----");
                if (num > 100000) {
                    break;
                }
            }
            FileWriter.put("/home/tj/big_data/data/talk/3j_2.csv",corpus2);
            System.out.println(String.format("%d", talks_map.size()));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public String lineFilter(String line) {
        line = line.replaceAll("\n", "");
        line = line.replaceAll(",", "，");
        return line;
    }
    public static void import_x() {
        FileReader.setFile_charset("UTF-8");
        List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/1/谭安军.txt", "txt");
        String regex = "---------(0|1)";
        Pattern p = Pattern.compile(regex);
        int total_num = 0, correct = 0;
        for (String sent : corpus) {
            Matcher m = p.matcher(sent);
            if (m.find()) {
                String s = sent.substring(sent.length()-1);
                if (s.equals("1")) {
                    correct++;
                }
                total_num++;
            }
        }
        System.out.println(String.format("%d/%d", correct, total_num));
    }
}
