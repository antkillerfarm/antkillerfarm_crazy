package oata;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.List;
import java.util.Random;

public class ServiceTrainer {
    boolean[] topic_flag;
    int current_topic;
    int current_sub_topic;
    int current_talk_index;
    boolean exit_flag = false;
    Random r = new Random();
    TalkAnalyzer talk_analyzer = null;

    public void dataPreprocess() {
        talk_analyzer = new TalkAnalyzer();
        talk_analyzer = talk_analyzer.load("/home/tj/big_data/data/talk/flower.model");
        talk_analyzer.clusteringByLDA();
        topic_flag = new boolean[talk_analyzer.topic_num];
        for (int i = 0; i < talk_analyzer.topic_num; i++) {
            if (talk_analyzer.topic_index.get(i).size() == 0) {
                topic_flag[i] = true;
            }
        }
        talk_analyzer.saveTalksForW2v("/home/tj/big_data/data/talk/flower_for_w2v.model");
        talk_analyzer.talksToD2v();
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

    public int selectSubTopicForTraining() {
        int num = r.nextInt(talk_analyzer.topic_index.get(current_topic).size());
        return num;
    }

    public void initRound() {
        current_sub_topic = selectSubTopicForTraining();
        current_talk_index = 0;
    }

    public void questionFromAI() {
        int talks_index = talk_analyzer.topic_index.get(current_topic).get(current_sub_topic);
        Talks talks = talk_analyzer.talks_for_train.get(talks_index);
        Talk talk = talks.talks.get(current_talk_index);
        while (!(talk_analyzer.isCustomer(talk.from) && talk.flag)) {
            current_talk_index++;
            talk = talks.talks.get(current_talk_index);
        }
        String str = talk.talk;
        System.out.println(String.format("Customer:%s", str));
    }

    public static String readString() {
        try {
            return new BufferedReader(new InputStreamReader(System.in)).readLine();
        } catch (Exception e) {
            e.printStackTrace();
            return ":exit";
        }
    }

    public boolean isAnswerMatch(String answer) {
        talk_analyzer.learn_doc.genDocVec(answer);
        return false;
    }

    public boolean handleAnswerFromService() {
        String answer = readString();
        if (answer.equals(":exit")) {
            exit_flag = true;
            return false;
        }
        if (isAnswerMatch(answer)) {
            current_talk_index += 2;
            return true;
        }
        List<Integer> topics = talk_analyzer.topic_index.get(current_topic);

        for (int i = 0; i < topics.size(); i++) {
            Integer topic = topics.get(i);
            Talks talks = talk_analyzer.talks_for_train.get(topic);

            for (int j = 0; j < talks.talks.size(); j++) {
                Talk talk = talks.talks.get(j);
                if (talk_analyzer.isCustomer(talk.from)) {
                    continue;
                }
                if (isAnswerMatch(answer)) {
                    current_sub_topic = i;
                    current_talk_index = 0;
                }
            }
        }

        return true;
    }

    public boolean trainRound() {
        showHelp();
        initRound();
        boolean res = true;
        while (res) {
            questionFromAI();
            res = handleAnswerFromService();
            if (exit_flag) {
                return true;
            }
        }
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

        service_trainer.current_topic = service_trainer.selectTopicForTraining();

        while (service_trainer.isAllTrained() == false) {
            service_trainer.current_topic = service_trainer.selectTopicForTraining();
            boolean result = service_trainer.trainRound();
            if (result) {
                break;
            }
        }
        System.out.println("Training Finished");
    }
}
