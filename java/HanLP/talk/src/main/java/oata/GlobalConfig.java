package oata;

public class GlobalConfig {

	public static String BASE_PATH = "c:\\robot";
	public static String DATA_PATH = BASE_PATH + "\\data";
	public static String BIN_PATH = DATA_PATH + "\\bin";
	public static String TRAIN_PATH = DATA_PATH + "\\train\\";
	
	/**
	 * 分词器
	 */
	public static String SEG_NAME = "hanlp";
	
	/**
	 * word2vector model path
	 */
	public static String W2V_CORPUS = TRAIN_PATH + "w2v\\";
	
	/**
	 * word2vector model path
	 */
	public static String W2V_MODEL = DATA_PATH + "/out/vector.mod";
	
	/**
	 * word2vector model templ result
	 */
	public static final String W2V_OUT = DATA_PATH + "/out/result.txt";
	
	/**
	 * hownet glossary
	 */
	public static String HOWNET_MODEL = BIN_PATH + "/hownet/glossary.dat";
	
	/*
	 * hownet whole
	 */
	public static String HOWNET_WHOLE = BIN_PATH + "/hownet/WHOLE.DAT";
	
	/**
	 * characor corpus path
	 */
	public static String CHARACTOR_CORPUS = TRAIN_PATH + "\\chs";
	
	/**
	 * charactor model path
	 */
	public static String CHARACTOR_MODEL = BASE_PATH + "/data/out/ccc2.bin";
	
	/**
	 * word2vector train corpus path
	 */
	public static String W2V_TRAIN_PATH = "";
	
	/**
	 * merge by lda train result path
	 */
	public static String LDA_PATH = DATA_PATH + "/out/lda.bin";
	
	/**
	 * dictionry path
	 */
	public static String CUSTOMER_DIC = DATA_PATH + "/dic/my.dic";
	
	/**
	 * dependency model path
	 */
	public static String DEPENDENCE_MODEL = BIN_PATH + "\\";
	
	/**
	 * train result file of sentence knowlege
	 */
	public static String SENTS_KNOWLEGE_PATH = DATA_PATH + "/out/sent.bin";
	
	/**
	 * train result file of sentiment knowlege
	 */
	public static String SENTI_KNOWLEGE_PATH = DATA_PATH + "/out/senti.bin";
	
	/**
	 * learn corpus path
	 */
	public static String LEARN_PATH = DATA_PATH + "\\learn\\";
		
	/**
	 * check corpus path
	 */
	public static String CHECK_PATH = DATA_PATH + "\\check\\";
	

	/**
	 * explain path
	 */
	public static String EXPLAIN_PATH = DATA_PATH + "\\know\\";
	
	/**
	 * sentiment vocabulary
	 * */
	public static String SENTI_VOC = BIN_PATH + "\\svoc.bin";
	
	/**
	 * sentiment bpn model
	 * */
	public static String SENTI_BPN = BIN_PATH + "\\bpn.bin";

	/**
	 * sentiment bpn model
	 * */
	public static String SENTI_TREE = BIN_PATH + "\\stree.bin";
	
	/**
	 * result set directory
	 */
	public static String RESULT_PATH = DATA_PATH + "\\result\\";
	
	public static String REMOVE_PATH = "";
	
	public static boolean DEBUG = false;
}
