package com.ansj.vec.LDA;

import org.apache.commons.lang3.StringUtils;

import java.io.IOException;
import java.util.List;
import java.util.Map;

public class AppForTest {
	public static void test1(){
		try {

			//Corpus corpus = Corpus.load("/home/tj/my/work/ai/tj-ai/tj-ai-lda/data/mini2");
			Corpus corpus = Corpus.loadFile("/home/tj/big_data/data/talk/x/flower_1.txt");
			LdaGibbsSampler ldaGibbsSampler = new LdaGibbsSampler(corpus.getDocument(), corpus.getVocabularySize());
			ldaGibbsSampler.gibbs(100);
			double[][] phi = ldaGibbsSampler.getPhi();

			Map<String, Double>[] topicMap = LdaUtil.translate(phi, corpus.getVocabulary(), 30);
			LdaUtil.explain(topicMap);

			/*int[] document = Corpus.loadDocument("/home/tj/my/work/ai/tj-ai/tj-ai-lda/data/mini2/军事_510.txt", corpus.getVocabulary());
			double[] tp = LdaGibbsSampler.inference(phi, document);
			Map<String, Double> topic = LdaUtil.translate(tp, phi, corpus.getVocabulary(), 10);
			LdaUtil.explain(topic);*/

			double[][] theta = ldaGibbsSampler.getTheta();
			int[] doc_topic = new int[theta.length];
			System.out.println(String.format("%d:%d", theta.length, theta[0].length));
			for (int m = 0; m < theta.length; m++) {
				double max = 0;
				int index = 0;
				for (int n = 0; n < theta[0].length; n++) {
					if (theta[m][n] > max) {
						max = theta[m][n];
						index = n;
					}
				}
				doc_topic[m] = index;
			}
			int num = 0;
			for (int m = 0; m < theta.length; m++) {
				if (doc_topic[m] == 1) {
					List<String> words = corpus.getDocumentContextList().get(m);
					System.out.println(StringUtils.join(words, " "));
					System.out.println("------------------");
					num++;
					if (num > 20) {
						break;
					}
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	public static void main(String[] args){
		test1();
	}
}
