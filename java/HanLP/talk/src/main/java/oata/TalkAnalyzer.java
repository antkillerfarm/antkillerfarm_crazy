package oata;

import com.ansj.vec.LDA.LDAService;
import com.ansj.vec.LDA.LdaUtil;
import com.ansj.vec.LDA.Vocabulary;
import com.ansj.vec.Learn;
import com.ansj.vec.LearnDocVec;
import com.ansj.vec.Word2VEC;
import com.ansj.vec.domain.Neuron;
import com.ansj.vec.domain.WordNeuron;
import com.ansj.vec.util.Haffman;
import com.ansj.vec.util.MapCount;
import com.hankcs.hanlp.HanLP;
import com.hankcs.hanlp.corpus.tag.Nature;
import com.hankcs.hanlp.seg.common.Term;
import joinery.DataFrame;
import org.apache.commons.lang3.StringUtils;

import java.io.*;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

class Talk implements Serializable{
    private static final long serialVersionUID = 1L;
    String from;
    String to;
    String talk;
    String talk_word_cut;
    String key_word;
    boolean flag;
    float[] doc_vec;
}
class Talks implements Serializable{
    private static final long serialVersionUID = 1L;
    int id;
    List<Talk> talks;
    String all_word_cut;
    String key_word;
}
class WordFreq implements Serializable{
    private static final long serialVersionUID = 1L;
    String word;
    int count;
}
class Topic implements Serializable{
    private static final long serialVersionUID = 1L;
    List<Talks> talks;
}
public class TalkAnalyzer implements Serializable {
    private static final long serialVersionUID = 1L;

    public int getTalks_id() {
        talks_id ++;
        return talks_id;
    }

    private int talks_id = 0;
    Map<String,Talks> talks_map = new HashMap<>();
    MapCount word_count_map = new MapCount();
    Map<String,Topic> topics_map = new HashMap<>();
    Map<String,Topic> out_topics_map = new HashMap<>();
    List<Talks> talks_for_train;
    List<Talk> all_talk_for_train;
    int topic_num = 100;
    List<List<Integer>> topic_index;
    Learn learn;
    LearnDocVec learn_doc;
    Word2VEC w2v;
    Word2VEC d2v;

    public boolean save(String path){
        try {
            ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(path));
            oos.writeObject(this);
            oos.flush();
            oos.close();

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    public TalkAnalyzer load(String path){
        ObjectInputStream oin = null;
        try{
            oin = new ObjectInputStream(new FileInputStream(path));
        } catch (Exception e1) {
            e1.printStackTrace();
        }

        TalkAnalyzer ins = null;
        try {
            ins = (TalkAnalyzer) oin.readObject();

        } catch (Exception e) {
            e.printStackTrace();
        }finally{
            try {
                if(oin != null)
                    oin.close();
            } catch (IOException ioe) {
            }
        }
        return ins;
    }

    public void import_flowerplus() {
        try {
            DataFrame<Object> df = DataFrame.readCsv("/home/tj/big_data/data/talk/3j.csv", ",", null, false);
            for (List<Object> row : df) {
                String session_id = (String)row.get(1);
                String from = (String)row.get(3);
                String to = (String)row.get(5);
                if (isIDValid(from) && isIDValid(to)) {
                    String key = session_id;//formKey(from, to);
                    Talks talks;
                    boolean first_flag = false;
                    if (!talks_map.containsKey(key)) {
                        talks = new Talks();
                        talks.talks = new LinkedList<>();
                        talks_map.put(key,talks);
                        first_flag = true;
                    } else {
                        talks = talks_map.get(key);
                    }
                    String s = (String)row.get(6);
                    if (s != null) {
                        Talk talk = new Talk();
                        if (first_flag) {
                            talk.flag = false;
                        }
                        else {
                            talk.flag = true;
                        }
                        talk.from = from;
                        talk.to = to;
                        talk.talk = lineFilter(s);
                        List<Term> terms = HanLP.segment(talk.talk);
                        s = "";
                        for (Term term : terms) {
                            if (term.nature != Nature.w) {
                                s += term.word + " ";
                                word_count_map.add(term.word);
                            }
                        }
                        talk.talk_word_cut = s;
                        talks.talks.add(talk);
                    }
                }
            }
            System.out.println("import finished");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void saveTalksForW2v(String file_name) {
        List<String> corpus =  new LinkedList<>();
        all_talk_for_train = new LinkedList<>();
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            Talks talks0 = talks.getValue();
            for (Talk talk : talks0.talks) {
                if (!talk.talk_word_cut.equals("")) {
                    corpus.add(talk.talk_word_cut);
                    all_talk_for_train.add(talk);
                }
            }
        }
        FileWriter.put(file_name,corpus);
    }

    public void saveTalksForRawText(String file_name) {
        List<String> corpus =  new LinkedList<>();
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            Talks talks0 = talks.getValue();
            for (Talk talk : talks0.talks) {
                if (!talk.talk_word_cut.equals("")) {
                    String str;
                    if (isCustomer(talk.from)) {
                        str = "Customer:";
                    }
                    else {
                        str = "Service:";
                    }
                    corpus.add(str + talk.talk);
                }
            }
            corpus.add("-----------------------------");
        }
        FileWriter.put(file_name,corpus);
    }

    public void talksToD2v() {
        try {
            File result = new File("/home/tj/big_data/data/talk/flower_for_w2v.model");
            /*BufferedReader br = new BufferedReader(new InputStreamReader(
                    new FileInputStream(result)));

            HashMap<String, Integer> lines = new HashMap<>();
            String temp = null;
            int num = 0;
            while ((temp = br.readLine()) != null) {
                lines.put(temp, num);
                num++;
            }
            String str = "需要 换 大 盒子 配送 的 ";
            Integer no = lines.get(str);*/

            learn = new Learn();
            learn.learnFile(result);
            learn.saveModel(new File("/home/tj/big_data/data/talk/flower_w2v.model"));
            //加载测试
            //learn.learnFileWithoutTrain(result);
            Map<String, Neuron> word2vec_model = learn.getWord2VecModel();
            learn_doc = new LearnDocVec(word2vec_model);
            learn_doc.learnFile(result);
            learn_doc.saveModel(new File("/home/tj/big_data/data/talk/flower_d2v.model"));
            w2v = new Word2VEC();
            w2v.loadJavaModel("/home/tj/big_data/data/talk/flower_d2v.model");
            d2v = new Word2VEC();
            d2v.loadJavaModel("/home/tj/big_data/data/talk/flower_w2v.model");
            /*float[] doc_vec = learn_doc.genDocVec(str);
            float[] doc_vec2 = w2v.getWordVector(String.format("%d",no));
            String[] strs = str.split(" ");
            float[] doc_vec4 = d2v.getWordsVector(strs);
            double sim = w2v.similarity(doc_vec, doc_vec2);
            System.out.println(String.format("%f", sim));
            sim = w2v.similarity(doc_vec2, doc_vec4);
            System.out.println(String.format("%f", sim));*/
            for (int i = 0; i < all_talk_for_train.size(); i++) {
                float[] doc_vec = w2v.getWordVector(String.format("%d",i));
                all_talk_for_train.get(i).doc_vec = doc_vec;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public boolean isIDValid(String role) {
        if (role.startsWith("wx") || role.startsWith("TJ_IM_WEB_WX_USER_ACCOUNT_")) {
            return true;
        }
        return false;
    }
    public boolean isCustomer(String role) {
        if (role.startsWith("wx")) {
            return true;
        }
        return false;
    }

    public String lineFilter(String line) {
        line = line.replaceAll("\n", "");
        line = line.replaceAll(",", "，");
        line = line.replaceAll("-", "");
        String regex1 = "[0-9A-Za-z\\t%<>#_]*";
        Pattern p1 = Pattern.compile(regex1);
        Matcher m1 = p1.matcher(line);
        line = m1.replaceAll("");
        /*line = line.replaceAll("\n", "");
        line = line.replaceAll("flowerplus", "XX");
        line = line.replaceAll("Flowerplus", "XX");
        line = line.replaceAll("FlowerPlus", "XX");
        line = line.replaceAll("花加", "XX");*/
        return line;
    }

    public List<WordFreq> getWordFreq() {
        List<Nature> natureList = Arrays.asList(Nature.vn);
        List<Map.Entry<String, Integer>> count_list = new ArrayList<Map.Entry<String, Integer>>(word_count_map.get().entrySet());
        List<WordFreq> word_freq = new LinkedList<>();
        for (Map.Entry<String, Integer> count : count_list) {
            String word = count.getKey();
            Nature nature = getWordNature(word);
            if (natureList.contains(nature)) {
                WordFreq wf = new WordFreq();
                wf.word = word;
                wf.count = count.getValue();
                word_freq.add(wf);
            }
        }
        Collections.sort(word_freq, new Comparator<WordFreq>() {
            public int compare(WordFreq o1, WordFreq o2) {
                return (o2.count - o1.count);
            }
        });
        return word_freq;
    }

    public List<WordFreq> getHighFreqWord(List<WordFreq> word_freq) {
        List<WordFreq> word_freq2 = new LinkedList<>();
        double total_count = 0;
        for (WordFreq wf : word_freq) {
            total_count += wf.count;
        }
        double part_count = total_count * 0.85;
        double part_count0 = 0;
        for (WordFreq wf : word_freq) {
            part_count0 += wf.count;
            word_freq2.add(wf);
            if (part_count0 >= part_count) {
                break;
            }
        }
        return word_freq2;
    }

    public Nature getWordNature(String word) {
        List<Term> terms = HanLP.segment(word);
        return terms.get(0).nature;
    }

    public void handleTalks() {
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            Talks talks0 = talks.getValue();
            talks0.all_word_cut = "";
            for (Talk talk : talks0.talks) {
                if (talk.flag == false) {
                    continue;
                }
                talks0.all_word_cut += talk.talk_word_cut + " ";
            }
            talks0.key_word = StringUtils.join(HanLP.extractKeyword(talks0.all_word_cut, 20), " ");
            talks0.id = getTalks_id();
        }
    }

    public void saveTalks(String file_name) {
        List<String> corpus =  new LinkedList<>();
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            Talks talks0 = talks.getValue();
            if (talks0.key_word != null) {
                corpus.add(talks0.key_word);
            }
        }
        FileWriter.put(file_name,corpus);
    }

    public void handleTopics(List<WordFreq> high_freq_word) {
        int len = high_freq_word.size();
        Map<String,String> talks_id_map = new HashMap<>();
        for (int i = len - 1; i >= 1; i--) {
            for (int j = i - 1; j >= 0; j--) {
                String keyword0 = high_freq_word.get(i).word;
                String keyword1 = high_freq_word.get(j).word;
                List<String> kw_list = Arrays.asList(keyword0, keyword1);
                handleTopic(kw_list, talks_id_map);
                String key = StringUtils.join(kw_list, " ");
                Topic topic = topics_map.get(key);
                clusteringByTopicDBScan(key, topic);
            }
        }
    }
    public void handleTopic(List<String> kw_list, Map<String,String> talks_id_map) {
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            Talks talks0 = talks.getValue();
            if (talks_id_map.containsKey(String.valueOf(talks0.id))) {
                continue;
            }
            int match_flag = 0;
            for (String kw : kw_list) {
                if (talks0.key_word.contains(kw)) {
                    match_flag++;
                }
            }
            if (match_flag == kw_list.size()) {
                String key = StringUtils.join(kw_list, " ");
                Topic topic;
                if (!topics_map.containsKey(key)) {
                    topic = new Topic();
                    topic.talks = new LinkedList<>();
                    topics_map.put(key,topic);
                } else {
                    topic = topics_map.get(key);
                }
                topic.talks.add(talks0);
                talks_id_map.put(String.valueOf(talks0.id), "");
            }
        }
    }
    public void clusteringByTopicDBScan2() {
        List<String> corpus =  new LinkedList<>();
        int num = 0;
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            if (talks.getValue().key_word == null) {
                continue;
            }
            corpus.add(talks.getValue().key_word);
            num++;
            if (num > 8000) {
                break;
            }
        }
        DBScan ds=new DBScan();
        ArrayList<DBScan.DataObject> source = ds.formDataObject(corpus);
        int clunum=ds.dbscan(source);

        int len = corpus.size();
        int num2 = 0;
        for (int i = 0; i < len; i++) {
            int cid = source.get(i).getCid();
            if (cid == -1) {
                num2++;
            }
            if (cid == 1) {
                System.out.println(source.get(i).getValue());
            }
        }
        System.out.println(String.format("%d/%d", clunum, num2));
    }

    public void clusteringByTopicDBScan(String key, Topic topic) {
        if (topic == null) {
            return;
        }
        List<String> corpus =  new LinkedList<>();
        for (Talks talks : topic.talks) {
            corpus.add(talks.key_word);
        }

        DBScan ds=new DBScan();
        ArrayList<DBScan.DataObject> source = ds.formDataObject(corpus);
        int clunum=ds.dbscan(source);

        Topic topic0;
        if (!out_topics_map.containsKey(key)) {
            topic0 = new Topic();
            topic0.talks = new LinkedList<>();
            out_topics_map.put(key,topic0);
        } else {
            topic0 = out_topics_map.get(key);
        }
        HashMap<String, String> cid_class = new HashMap<>();
        int len = corpus.size();
        for (int i = 0; i < len; i++) {
            int cid = source.get(i).getCid();
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
                topic0.talks.add(topic.talks.get(i));
            }
        }
        System.out.println(String.format("%d/%d/%d", clunum, topic0.talks.size(), topic.talks.size()));
    }

    public void output_report() {
        List<String> corpus = new LinkedList<>();
        String s;
        int num = 0;
        for (Map.Entry<String,Topic> topic : out_topics_map.entrySet()) {
            List<Talks> talks_list = topic.getValue().talks;
            s = "--------------------\n";
            s += String.format("%s:%d\n", topic.getKey(), talks_list.size());
            s += "--------------------\n";
            corpus.add(s);
            for (Talks talks : talks_list) {
                for (Talk talk : talks.talks) {
                    corpus.add(talk.talk);
                }
                s = "--------------------\n";
                corpus.add(s);
                num++;
            }
        }
        corpus.add(String.format("%d/%d/%d", out_topics_map.size(), num, talks_map.size()));
        FileWriter.put("/home/tj/big_data/data/talk/2j3sc_4_6.txt",corpus);
    }

    public void fetchFAQ(boolean is_customer) {
        int key_word_threshold = 1;
        int start_threshold = 5;
        int reply_threshold = 10;
        MapCount kw_count_map = new MapCount();
        MapCount kw_start_count_map = new MapCount();
        Map<String,String> talk_map = new HashMap<>();
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            int len = talks.getValue().talks.size();
            for (int i = 0; i < len; i++) {
                Talk talk = talks.getValue().talks.get(i);
                if (is_customer) {
                    if (!isCustomer(talk.from)) {
                        continue;
                    }
                } else {
                    if (isCustomer(talk.from)) {
                        continue;
                    }
                }
                List<String> kw_list = HanLP.extractKeyword(talk.talk_word_cut, 15);
                if (kw_list.size() < key_word_threshold) {
                    continue;
                }
                talk.key_word = StringUtils.join(kw_list, " ");
                kw_count_map.add(talk.key_word);
                if (!talk_map.containsKey(talk.key_word)) {
                    talk_map.put(talk.key_word, talk.talk);
                }
                if (i < start_threshold) {
                    kw_start_count_map.add(talk.key_word);
                }
            }
        }
        List<Map.Entry<String, Integer>> kw_count_list = new ArrayList<Map.Entry<String, Integer>>(kw_count_map.get().entrySet());
        Collections.sort(kw_count_list, new Comparator<Map.Entry<String, Integer>>() {
            public int compare(Map.Entry<String, Integer> o1, Map.Entry<String, Integer> o2) {
                return (o2.getValue() - o1.getValue());
            }
        });
        List<String> auto_reply = new LinkedList<>();
        List<String> normal_reply = new LinkedList<>();
        for (int i = 0; i < kw_count_list.size(); i++) {
            String key = kw_count_list.get(i).getKey();
            int total_num = kw_count_list.get(i).getValue();
            if (total_num <= reply_threshold) {
                continue;
            }
            int num = 0;
            if (kw_start_count_map.get().containsKey(key)) {
                num = (int)kw_start_count_map.get().get(key);
            }
            double percent = 1.0 * num / total_num;
            if (percent > 0.7) {
                //auto_reply.add(talk_map.get(key));
                auto_reply.add(key);
            } else {
                //normal_reply.add(talk_map.get(key));
                normal_reply.add(key);
            }
        }
        filterFAQ(auto_reply);
        /*List<String> corpus = new LinkedList<>();
        String s;
        s = "--------------------\n";
        s += String.format("自动回复:%d\n", auto_reply.size());
        s += "--------------------\n";
        corpus.add(s);
        corpus.addAll(auto_reply);
        s = "--------------------\n";
        s += String.format("非自动回复:%d\n", normal_reply.size());
        s += "--------------------\n";
        corpus.add(s);
        corpus.addAll(normal_reply);
        String file;
        if (is_customer) {
            file = "/home/tj/big_data/data/talk/2j3sc_4_customer.txt";
        } else {
            file = "/home/tj/big_data/data/talk/2j3sc_4_service.txt";
        }

        FileWriter.put(file, corpus);
        System.out.println("bye");*/

        System.out.println("fetchFAQ");
    }

    public void filterFAQ(List<String> faqs) {
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            List<Talk> talks0 = talks.getValue().talks;
            for (int i = 0; i < talks0.size(); i++) {
                Talk talk = talks0.get(i);
                for (String faq : faqs) {
                    if (talk.key_word == null) {
                        //talk.flag = false;
                        continue;
                    }
                    //if (talk.key_word.equals(faq)) {
                    if (DBScan.calJaccardDist(talk.key_word, faq) < 0.2 && i <= 5) {
                        talk.flag = false;
                    }
                }
            }
        }
    }

    public void clusteringByLDA() {
        LDAService lda = LDAService.getInst();
        List<List<String>> docs = new LinkedList<>();
        talks_for_train = new LinkedList<>();
        for (Map.Entry<String, Talks> talks : talks_map.entrySet()) {
            Talks talks0 = talks.getValue();
            if (talks0 != null) {
                if (talks0.key_word == null || talks0.key_word.equals("")) {
                    continue;
                }
                List<String> doc = Arrays.asList(talks0.all_word_cut.split(" "));
                docs.add(doc);
                talks_for_train.add(talks0);
            }
        }
        //lda.trainEx(docs,topic_num);
        //lda.save("/home/tj/big_data/data/talk/flower_lda.model");

        lda.load("/home/tj/big_data/data/talk/flower_lda.model");
        //double[][] phi = lda.getPhi();
        //Map<String, Double>[] topicMap = LdaUtil.translate(phi, corpus.getVocabulary(), 30);
        //LdaUtil.explain(topicMap);
        double[][] theta = lda.getTheta();
        topic_index = new ArrayList<>();
        for (int n = 0; n < topic_num; n++) {
            List<Integer> indexs = new LinkedList<>();
            topic_index.add(indexs);
        }
        for (int m = 0; m < theta.length; m++) {
            double max = 0;
            int index = 0;
            for (int n = 0; n < theta[0].length; n++) {
                if (theta[m][n] > max) {
                    max = theta[m][n];
                    index = n;
                }
            }
            topic_index.get(index).add(m);
        }
        /*System.out.println(String.format("%d", talks_for_train.size()));
        for (int n = 0; n < topic_num; n++) {
            System.out.println(String.format("%d:%d", n, topic_index.get(n).size()));
        }
        for (int n = 0; n < 10; n++) {
            System.out.println(talks_for_train.get(topic_index.get(1).get(n)).all_word_cut);
        }*/
    }

    public void outputLDAWordClass() {
        Vocabulary vocab = Vocabulary.getInst();
        vocab.load("/home/tj/big_data/data/talk/flower_vocabulary.model");
        LDAService lda = LDAService.getInst();
        double[][] phi = lda.getPHI();
        Map<String, Double>[] topicMap = LdaUtil.translate(phi, vocab, 30);
        LdaUtil.explain(topicMap);
    }

    public static void main(String[] args) {
        TalkAnalyzer app = new TalkAnalyzer();

        /*long startTime=System.nanoTime();
        app.import_flowerplus();
        long endTime= System.nanoTime();
        System.out.println(String.format("step1:%d", endTime - startTime));
        startTime = endTime;
        app.fetchFAQ(false);
        endTime= System.nanoTime();
        System.out.println(String.format("step2:%d", endTime - startTime));
        startTime = endTime;
        app.handleTalks();
        //app.saveTalks("/home/tj/big_data/data/talk/flower_1.txt");
        endTime= System.nanoTime();
        System.out.println(String.format("step3:%d", endTime - startTime));
        startTime = endTime;
        app.save("/home/tj/big_data/data/talk/flower.model");
        endTime= System.nanoTime();
        System.out.println(String.format("step4:%d", endTime - startTime));*/
        app = app.load("/home/tj/big_data/data/talk/flower.model");
        app.saveTalksForW2v("/home/tj/big_data/data/talk/flower_for_w2v.model");
        app.talksToD2v();
        //app.clusteringByLDA();
        //app.outputLDAWordClass();
        //app.clusteringByTopicDBScan2();

        //app.fetchFAQ(true);
        //app.fetchFAQ(false);
        /*List<WordFreq> word_freq = app.getWordFreq();
        List<WordFreq> high_freq_word = app.getHighFreqWord(word_freq);
        System.out.println("step1");
        app.handleTalks();
        System.out.println("step2");
        app.handleTopics(high_freq_word);
        System.out.println("step3");
        app.output_report();*/
    }
}
