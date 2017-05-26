package oata;

import java.util.Random;

public class ServiceTrainer {
    boolean[] topic_flag;
    int current_topic;
    Random r = new Random();
    TalkAnalyzer talk_analyzer = null;

    public void dataPreprocess() {
        talk_analyzer = new TalkAnalyzer();
        talk_analyzer = talk_analyzer.load("/home/tj/big_data/data/talk/flower.model");
        talk_analyzer.clusteringByLDA();
        topic_flag = new boolean[talk_analyzer.topic_num];
    }

    public int selectTopicForTraining() {
        int num = r.nextInt(talk_analyzer.topic_num);
        while (topic_flag[num]) {
            num = r.nextInt(talk_analyzer.topic_num);
        }
        topic_flag[num] = true;
        return num;
    }

    public boolean isAllTrained() {
        for (boolean flag : topic_flag) {
            if (flag == false) {
                return false;
            }
        }
        return true;
    }

    public boolean trainRound() {
        showHelp();

        return false;
    }

    private void showHelp() {
        System.out.println("input :exit to exit");
    }

    public static void main(String[] args) {
        System.out.println("welcome to ServiceTrainer, version 1.0.0");
        System.out.println("Loading.....");
        ServiceTrainer service_trainer = new ServiceTrainer();
        service_trainer.dataPreprocess();
        System.out.println("Loading Finished");

        /*service_trainer.current_topic = service_trainer.selectTopicForTraining();

        while (service_trainer.isAllTrained() == false) {
            service_trainer.current_topic = service_trainer.selectTopicForTraining();
            boolean result = service_trainer.trainRound();
            if (result) {
                break;
            }
        }
        System.out.println("Training Finished");*/
    }
}
