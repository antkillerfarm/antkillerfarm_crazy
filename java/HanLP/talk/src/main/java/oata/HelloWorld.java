package oata;

import java.io.*;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.ansj.vec.Learn;
import com.ansj.vec.LearnDocVec;
import com.ansj.vec.domain.Neuron;
import com.ansj.vec.domain.WordEntry;
import com.ansj.vec.util.MapCount;
import com.ansj.vec.util.WordKmeans;
import com.hankcs.hanlp.HanLP;
import com.hankcs.hanlp.corpus.dependency.CoNll.CoNLLSentence;
import com.hankcs.hanlp.corpus.tag.Nature;
import com.hankcs.hanlp.seg.common.Term;
import joinery.DataFrame;
import org.apache.commons.lang3.StringUtils;
import com.ansj.vec.Word2VEC;

public class HelloWorld {

    public static void main(String[] args) {
		/*step1();
		step2();
		step3();
		test12();
		test13();
		test14();
		test15();*/
		//step1_0();
		test8_2_2();
		/*step1_2();
		step1_3();
		step3();
		test12();
		test13();
		test14();
		test15();*/
	}
	public static void step1_0() {
		FileReader.setFile_charset("UTF-8");
		List<String> corpus = FileReader.getAll("/home/tj/big_data/data/jinyong/utf8/1.txt", "txt");
		List<String> corpus2 =  new LinkedList<>();
		for(String sent : corpus){
			List<Term> keywordList = HanLP.segment(sent);
			String keywordStr = "";
			for (Term term : keywordList) {
				if (term.nature != Nature.w) {
					keywordStr += term.word + " ";
				}
			}
			corpus2.add(keywordStr);
		}
		FileWriter.put("/home/tj/big_data/data/jinyong/utf8/2.txt", corpus2);
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
	public static void step1_2() {
		FileReader.setFile_charset("UTF-8");
		List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/tj_tjk_customer_IM_msg.csv", "txt");
		List<String> corpus2 =  new LinkedList<>();
		for(int i = 0; i < corpus.size(); i++){
			String sent = corpus.get(i);
			corpus2.add(sent);
		}
		FileWriter.put("/home/tj/big_data/data/talk/3j.csv",corpus2);
	}
	public static void step1_3() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/3j.csv", ",",null,false);
			DataFrame<Object> df2 = new DataFrame<>("role","talk");
			int df_len = df.length();
			for (int i = 26000; i < df_len; i++) {
				String talk = (String)df.get(i,1);
				if (talk == null) {
					continue;
				}
				talk = talk.replaceAll("\n", "");
				talk = talk.replaceAll("【收到不支持的消息类型，暂无法显示】", "");
				talk = talk.replaceAll("【收到不支援的訊息類型，無法顯示】", "");
				talk = talk.replaceAll(",", "，");
				Long role0 = (Long)df.get(i,2);
				String role;
				if (role0 == 0) {
					role = "C";
				} else {
					role = "S";
				}
				if (!talk.equals("")) {
					df2.append(Arrays.asList(role, talk));
				}
			}
			df2.writeCsv("/home/tj/big_data/data/talk/2j2.csv");
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
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
	public static void step2_1() {
		FileReader.setFile_charset("UTF-8");
		List<String> corpus = FileReader.getAll("/home/tj/big_data/data/talk/2j.txt", "txt");
		if (corpus == null) return;
		List<String> corpus2 =  new LinkedList<>();
		boolean start_flag = false;
		boolean first_flag = false;
		String s_name = null;
		String current_role = null;
		//int num = 0;
		//String str = "";
		String talk_all = "";
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
				if (start_flag) {
					if (current_role!=null) {
						String str = current_role + "," + talk_all;
						corpus2.add(str);
					}
					talk_all = "TAGSTART，";
					start_flag = false;
					//String str = current_role + "," + talk_all;
					//corpus2.add(str);
					current_role = null;
				}
				int pos1 = StringUtils.ordinalIndexOf(sent, " ", 2);
				int pos2 = StringUtils.ordinalIndexOf(sent, " ", 3);
				if (pos2 == -1)
				{
					continue;
				}
				String name = sent.substring(pos1+1,pos2-1);
				String talk = sent.substring(pos2+1);
				boolean flag2 = lineFilter(talk);

				String role;
				if (name.contains(s_name)) {
					role = "S";
				} else {
					role = "C";
				}
				if (current_role != null && !current_role.equals(role))
				{
					String str = current_role + "," + talk_all;
					corpus2.add(str);
					talk_all = "";
				}
				if (!flag2 && talk.length() > 0) {
					talk_all += talk;
					//corpus2.add(str);
				}
				current_role = role;
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
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2.csv", ",",null,false);
			DataFrame<Object> df2 = new DataFrame<>("keyword");
			int row_num = 0;
			for (List<Object> row : df) {
				String talk = (String)row.get(1);
				if (talk==null)
				{
					talk = "";
				}
				List<String> keywordList = HanLP.extractKeyword(talk, 20);
				String[] talk_kw_list = keywordList.toArray(new String[keywordList.size()]);
				Arrays.sort(talk_kw_list);
				String keywordStr = "";
				for (String kw : talk_kw_list) {
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
			df3.writeCsv("/home/tj/big_data/data/talk/2j3_3.csv");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void step3_2() {
		try {
			HanLP.Config.NNParserModelPath = "/home/tj/big_data/data/HanLP/" + HanLP.Config.NNParserModelPath;
			String regex = formQuestionRegex();
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j2.csv", ",",null,false);
			DataFrame<Object> df2 = new DataFrame<>("keyword","class");
			int row_num = 0;
			double t1= 0, t2 = 0, t3 = 0;
			for (List<Object> row : df) {
				String talk = (String)row.get(1);
				if (talk==null)
				{
					talk = "";
				}
				List<String> keywordList = HanLP.extractKeyword(talk, 20);
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
				List<String> keywordList = HanLP.extractKeyword(talk, 20);
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
	public static void test8() {
		try {
			File result = new File("/home/tj/big_data/data/talk/2j3s.csv");
			Learn learn = new Learn();

			// 训练词向量
			learn.learnFile(result);
			learn.saveModel(new File("/home/tj/big_data/data/talk/2j3s.mod"));

			Map<String, Neuron> word2vec_model = learn.getWord2VecModel();
			LearnDocVec learn_doc = new LearnDocVec(word2vec_model);
			learn_doc.learnFile(result);
			learn_doc.saveModel(new File("/home/tj/big_data/data/talk/2j3s.mod.bin"));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test8_1() {
		try {
			File result = new File("/home/tj/big_data/data/talk/2j3s.csv");
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(result)));
			List<String> lines = new ArrayList<>();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				lines.add(temp);
			}

			Word2VEC doc_vec = new Word2VEC();
			doc_vec.loadJavaModel("/home/tj/big_data/data/talk/2j3s.mod.bin");

			System.out.println("welcome to tj.ai, version 1.0.0");
			System.out.println("please input command to act, type exit for exit");

			String cmd = readString();
			while (!cmd.equals("exit")) {
				//get short command
				queryDoc(Integer.parseInt(cmd), doc_vec, lines);
				cmd = readString();
			}

			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test8_2() {
		try {
			File result = new File("/home/tj/big_data/data/talk/2j3s.csv");
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(result)));
			List<String> lines = new ArrayList<>();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				lines.add(temp);
			}

			Word2VEC word_vec = new Word2VEC();
			word_vec.loadJavaModel("/home/tj/big_data/data/talk/2j3s.mod");
			Word2VEC doc_vec = new Word2VEC();
			doc_vec.loadJavaModel("/home/tj/big_data/data/talk/2j3s.mod.bin");

			System.out.println("welcome to tj.ai, version 1.0.0");
			System.out.println("please input command to act, type exit for exit");

			String cmd = readString();
			while (!cmd.equals("exit")) {
				//get short command
				queryDocByKey(cmd, word_vec, doc_vec, lines);
				cmd = readString();
			}

			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test8_2_2() {
		try {
			File result = new File("/home/tj/big_data/data/talk/flower_for_w2v.model");
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(result)));
			List<String> lines = new ArrayList<>();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				lines.add(temp);
			}

			Learn learn = new Learn();
			learn.learnFile(result);
			learn.saveModel(new File("/home/tj/big_data/data/talk/flower_w2v.model"));
			//加载测试
			//learn.learnFileWithoutTrain(result);
			Map<String, Neuron> word2vec_model = learn.getWord2VecModel();
			LearnDocVec learn_doc = new LearnDocVec(word2vec_model);
			learn_doc.learnFile(result);
			learn_doc.saveModel(new File("/home/tj/big_data/data/talk/flower_d2v.model"));
			Word2VEC word_vec = new Word2VEC();
			word_vec.loadJavaModel("/home/tj/big_data/data/talk/flower_w2v.model");
			Word2VEC doc_vec = new Word2VEC();
			doc_vec.loadJavaModel("/home/tj/big_data/data/talk/flower_d2v.model");

			System.out.println("welcome to tj.ai, version 1.0.0");
			System.out.println("please input command to act, type exit for exit");

			String cmd = readString();
			while (!cmd.equals("exit")) {
				//get short command
				//queryDocByKey(cmd, word_vec, doc_vec, lines);
				queryDocByKey2(cmd, word_vec, doc_vec, lines, learn_doc);
				cmd = readString();
			}

			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static String readString() {
		try {
			return new BufferedReader(new InputStreamReader(System.in)).readLine();
		} catch (Exception e) {
			e.printStackTrace();
			return "exit";
		} finally {

		}
	}

	public static void queryDoc(int query_no, Word2VEC doc_vec, List<String> lines) {
		Set<WordEntry> doc_entry = doc_vec.distance(String.format("%d", query_no));
		System.out.println(String.format("%d:%s", query_no, lines.get(query_no)));
		for (WordEntry doc : doc_entry) {
			//System.out.println(doc.name);
			System.out.println(String.format("%s:%f:%s", doc.name, doc.score, lines.get(Integer.parseInt(doc.name))));
		}
	}

	public static void queryDocByKey(String key, Word2VEC word_vec, Word2VEC doc_vec, List<String> lines) {
		String[] keys = StringUtils.split(key, " ");
		float[] center = word_vec.getWordsVector(keys);
		Set<WordEntry> doc_entry = doc_vec.distance(center);
		//System.out.println(String.format("%d:%s", query_no, lines.get(query_no)));
		for (WordEntry doc : doc_entry) {
			//System.out.println(doc.name);
			System.out.println(String.format("%s:%f:%s", doc.name, doc.score, lines.get(Integer.parseInt(doc.name))));
		}
	}
	public static void queryDocByKey2(String key, Word2VEC word_vec, Word2VEC doc_vec, List<String> lines, LearnDocVec learn_doc) {
		float[] center = learn_doc.genDocVec(key);
		Set<WordEntry> doc_entry = doc_vec.distance(center);
		//System.out.println(String.format("%d:%s", query_no, lines.get(query_no)));
		for (WordEntry doc : doc_entry) {
			//System.out.println(doc.name);
			System.out.println(String.format("%s:%f:%s", doc.name, doc.score, lines.get(Integer.parseInt(doc.name))));
		}
	}
	public static void test9() {
		try {
			Word2VEC vec = new Word2VEC();
			vec.loadJavaModel("/home/tj/big_data/data/talk/2j3s.mod.bin");
			System.out.println("load model ok!");
			WordKmeans wordKmeans = new WordKmeans(vec.getWordMap(), 50, 50);
			WordKmeans.Classes[] explain = wordKmeans.explain();

			for (int i = 0; i < explain.length; i++) {
				System.out.println("--------" + i + "---------");
				System.out.println(explain[i].getTop(10));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test10() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);
			System.out.println("welcome to tj.ai, version 1.0.0");
			System.out.println("please input command to act, type exit for exit");

			String cmd = readString();
			while (!cmd.equals("exit")) {
				//get short command
				queryKeyword(df, cmd);
				cmd = readString();
			}

			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void queryKeyword(DataFrame<Object> df, String keyword) throws IOException {
		DataFrame<Object> df2 = new DataFrame<>("No","talk");
		int row_num = 0;
		for (List<Object> row : df) {
			String talk_keyword = (String)row.get(2);
			if (keyword.equals(talk_keyword)) {
				String talk = (String)row.get(1);
				System.out.println(String.format("%d:%s", row_num+1, talk));
				df2.append(Arrays.asList(row_num + 1, talk));
			}
			row_num++;
		}
		df2.writeCsv("/home/tj/big_data/data/talk/2j3_4.csv");
	}
	public static void test11() {
		try {
			File result = new File("/home/tj/big_data/data/talk/2j3s.csv");
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(result)));
			List<String> lines = new ArrayList<>();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				lines.add(temp);
			}

			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_4.csv", ",",null,true);
			HashMap<String,float[]> doc_vec_map = new HashMap<>();

			Word2VEC vec = new Word2VEC();
			vec.loadJavaModel("/home/tj/big_data/data/talk/2j3s.mod.bin");
			System.out.println("load model ok!");

			HashMap<String, float[]> vec_map = vec.getWordMap();
			for (List<Object> row : df) {
				String talk_no = row.get(0).toString();
				float[] talk_vec = vec_map.get(talk_no);
				doc_vec_map.put(talk_no, talk_vec);
			}

			WordKmeans wordKmeans = new WordKmeans(doc_vec_map, 5, 50);
			WordKmeans.Classes[] explain = wordKmeans.explain();

			for (int i = 0; i < explain.length; i++) {
				System.out.println("--------" + i + "---------");
				List<Map.Entry<String, Double>> explain_list = explain[i].getTop(10);
				for(Map.Entry<String, Double> explain_item : explain_list) {
					String talk_no = explain_item.getKey();
					System.out.println(lines.get(Integer.parseInt(talk_no)));
				}

			}

			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test12() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);
			DataFrame<Object> df2 = df.groupBy(2).count().sortBy(-1);
			DataFrame<Object> df3 = new DataFrame<>("talk","count");

			for (List<Object> row : df2) {
				String talk_keyword = (String)row.get(0);
				int talk_count = (int)row.get(1);
				String[] talk_kw_list = StringUtils.split(talk_keyword, " ");
				if (talk_kw_list != null) {
					if (talk_kw_list.length > 2 && talk_count > 9) {
						df3.append(Arrays.asList(talk_keyword, talk_count));
					}
				}
			}

			df3.writeCsv("/home/tj/big_data/data/talk/2j3_2.csv");
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test13() {
		try {
			File result = new File("/home/tj/big_data/data/talk/2j3s.csv");
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(result)));
			List<String> lines = new ArrayList<>();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				lines.add(temp);
			}

			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_2.csv", ",",null,true);
			DataFrame<Object> df2 = DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);
			DataFrame<Object> df3 = new DataFrame<>("talk");

			for (List<Object> row : df) {
				String talk_keyword = (String)row.get(0);
				for (List<Object> row2 : df2) {
					String talk_keyword2 = (String)row2.get(2);
					if (talk_keyword.equals(talk_keyword2)) {
						String talk = (String)row2.get(1);
						df3.append(Arrays.asList(talk));
						break;
					}
				}
			}

			df3.writeCsv("/home/tj/big_data/data/talk/2j3_5.csv");
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test13_1() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_2.csv", ",",null,true);
			DataFrame<Object> df2 = DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);
			DataFrame<Object> df3 = new DataFrame<>("talk", "count","start_count","class");
			HashMap<String, String> talk_class = new HashMap<>();

			for (List<Object> row : df) {
				String talk_keyword = (String)row.get(0);
				Long count = (Long)row.get(1);
				int df2_len = df2.length();
				int start_count = 0;
				for (int i = 0; i < df2_len; i++) {
					String talk_keyword2 = (String)df2.get(i,2);
					if (talk_keyword.equals(talk_keyword2)) {
						for (int j = 0; j < 6; j++) {
							int index = i - j;
							if (index < 0 || index >= df2_len) {
								break;
							}
							String talk = (String) df2.get(index, 1);
							if (talk.contains("TAGSTART")) {
								start_count++;
								break;
							}
						}
					}
				}
				if ((double)(start_count)/count > 0.8) {
					if (!talk_class.containsKey(talk_keyword)) {
						df3.append(Arrays.asList(talk_keyword, count, start_count, "Start"));
						talk_class.put(talk_keyword,"Start");
					}
				}
			}

			for (List<Object> row : df) {
				String talk_keyword = (String)row.get(0);
				if (talk_class.containsKey(talk_keyword)) {
					continue;
				}
				Long count = (Long)row.get(1);
				int df2_len = df2.length();
				int start_count = 0;
				for (int i = 0; i < df2_len; i++) {
					String talk_keyword2 = (String)df2.get(i,2);
					if (talk_keyword.equals(talk_keyword2)) {
						for (int j = 1; j < 6; j++) {
							int index = i + j;
							if (index < 0 || index >= df2_len) {
								break;
							}
							String talk = (String) df2.get(index, 1);
							if (talk.contains("TAGSTART")) {
								start_count++;
								break;
							}
						}
					}
				}
				if ((double)(start_count)/count > 0.8) {
					if (!talk_class.containsKey(talk_keyword)) {
						df3.append(Arrays.asList(talk_keyword, count, start_count, "End"));
						talk_class.put(talk_keyword,"End");
					}
				}
			}
			for (List<Object> row : df) {
				String talk_keyword = (String) row.get(0);
				Long count = (Long)row.get(1);
				if (!talk_class.containsKey(talk_keyword)) {
					//continue;
					df3.append(Arrays.asList(talk_keyword, count, 0, "Unknown"));
				}
			}
			df3.writeCsv("/home/tj/big_data/data/talk/2j3_2_2.csv");
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test13_2() {
		try {
			File result = new File("/home/tj/big_data/data/talk/2j3s.csv");
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(result)));
			List<String> lines = new ArrayList<>();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				lines.add(temp);
			}

			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_2_2.csv", ",",null,true);
			DataFrame<Object> df2 = DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);
			DataFrame<Object> df3 = new DataFrame<>("talk","class");

			for (List<Object> row : df) {
				String talk_keyword = (String)row.get(0);
				String talk_type = (String)row.get(3);
				for (List<Object> row2 : df2) {
					String talk_keyword2 = (String)row2.get(2);
					if (talk_keyword.equals(talk_keyword2)) {
						String talk = (String)row2.get(1);
						df3.append(Arrays.asList(talk,talk_type));
						break;
					}
				}
			}

			df3.writeCsv("/home/tj/big_data/data/talk/2j3_5_2.csv");
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test14() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);
			MapCount kw_count_map = new MapCount();
			for (List<Object> row : df) {
				String talk_keyword = (String)row.get(2);
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
			List<Nature> natureList = Arrays.asList(Nature.vn);
			double total_count = 0;
			for (int i = 0; i < kw_count_list.size(); i++) {
				//String id = kw_count_list.get(i).toString();
				//System.out.println(id);
				String key = kw_count_list.get(i).getKey();
				List<Term> term_list = HanLP.segment(key);
				for (Term term : term_list) {
					//System.out.println(term.word + "/" + term.nature);
					if (natureList.contains(term.nature)) {
						//df2.append(Arrays.asList(key + "/" + term.nature, kw_count_list.get(i).getValue()));
						total_count += kw_count_list.get(i).getValue();
					}
				}
			}
			System.out.println(total_count);
			double part_count = total_count * 0.85;
			double part_count0 = 0;
			outf:for (int i = 0; i < kw_count_list.size(); i++) {
				//String id = kw_count_list.get(i).toString();
				//System.out.println(id);
				String key = kw_count_list.get(i).getKey();
				List<Term> term_list = HanLP.segment(key);
				for (Term term : term_list) {
					//System.out.println(term.word + "/" + term.nature);
					if (natureList.contains(term.nature)) {
						df2.append(Arrays.asList(key, kw_count_list.get(i).getValue()));
						part_count0 += kw_count_list.get(i).getValue();
						if (part_count0 >= part_count) {
							break outf;
						}
					}
				}
			}
			DataFrame<Object> df3 = df2.sortBy(1);
			df3.writeCsv("/home/tj/big_data/data/talk/2j3_6.csv");
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test15() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_5.csv", ",",null,true);
			DataFrame<Object> df2 =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_6.csv", ",",null,true);
			//DataFrame<Object> df3 = df2.sortBy(-1);

			File output_file = new File("/home/tj/big_data/data/talk/2j3_7.txt");
			OutputStream out = new FileOutputStream(output_file, false);
			BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(out,
					"utf-8"));

			HashMap<String, String> talk_class = new HashMap<>();

			for (List<Object> row2 : df2) {
				String kw = (String)row2.get(0);
				String s = "--------------------\n";
				s += kw + "\n";
				s += "--------------------\n";
				bw.write(s);
				for (List<Object> row : df) {
					String talk = (String)row.get(0);
					if (talk.contains(kw)) {
						if (!talk_class.containsKey(talk)) {
							talk_class.put(talk,kw);
							bw.write(talk + "\n");
						}
					}
				}
			}
			String s = "--------------------\n";
			s += "其他\n";
			s += "--------------------\n";
			bw.write(s);
			for (List<Object> row : df) {
				String talk = (String)row.get(0);
				if (!talk_class.containsKey(talk)) {
					bw.write(talk + "\n");
				}
			}
			//bw.write(sb.toString());
			bw.close();
			out.close();
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test15_2() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_5_2.csv", ",",null,true);
			DataFrame<Object> df2 =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_6.csv", ",",null,true);
			//DataFrame<Object> df3 = df2.sortBy(-1);

			File output_file = new File("/home/tj/big_data/data/talk/2j3_7.txt");
			OutputStream out = new FileOutputStream(output_file, false);
			BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(out,
					"utf-8"));

			HashMap<String, String> talk_class = new HashMap<>();
			String s = "--------------------\n";
			s += "欢迎词" + "\n";
			s += "--------------------\n";
			bw.write(s);
			for (List<Object> row : df) {
				String talk_type = (String)row.get(1);
				if (talk_type.equals("Start")){
					String talk = (String)row.get(0);
					talk_class.put(talk, "欢迎词");
					bw.write(talk + "\n");
				}
			}

			s = "--------------------\n";
			s += "结束语" + "\n";
			s += "--------------------\n";
			bw.write(s);
			for (List<Object> row : df) {
				String talk_type = (String)row.get(1);
				if (talk_type.equals("End")){
					String talk = (String)row.get(0);
					talk_class.put(talk, "结束语");
					bw.write(talk + "\n");
				}
			}

			for (List<Object> row2 : df2) {
				String kw = (String)row2.get(0);
				s = "--------------------\n";
				s += kw + "\n";
				s += "--------------------\n";
				bw.write(s);
				for (List<Object> row : df) {
					String talk = (String)row.get(0);
					if (talk.contains(kw)) {
						if (!talk_class.containsKey(talk)) {
							talk_class.put(talk,kw);
							bw.write(talk + "\n");
						}
					}
				}
			}
			s = "--------------------\n";
			s += "其他\n";
			s += "--------------------\n";
			bw.write(s);
			for (List<Object> row : df) {
				String talk = (String)row.get(0);
				if (!talk_class.containsKey(talk)) {
					bw.write(talk + "\n");
				}
			}
			//bw.write(sb.toString());
			bw.close();
			out.close();
			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public static void test16() {
		try {
			DataFrame<Object> df =  DataFrame.readCsv("/home/tj/big_data/data/talk/2j3_3.csv", ",",null,true);

			System.out.println("bye");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
