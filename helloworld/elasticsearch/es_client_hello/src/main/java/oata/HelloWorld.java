package oata;

import java.util.Collections;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import com.fasterxml.jackson.databind.JsonNode;
import org.apache.http.HttpHost;
import org.apache.http.entity.ContentType;
import org.apache.http.nio.entity.NStringEntity;
import org.elasticsearch.client.Response;
import org.elasticsearch.client.RestClient;
import org.apache.commons.io.IOUtils;
import com.fasterxml.jackson.databind.ObjectMapper;

public class HelloWorld {
    public static RestClient client;

    public static void main(String[] args) {
        System.out.println("Hello World");
        client = RestClient.builder(new HttpHost("localhost", 9200, "http")).build();

        try {
            //test1();
            //test2();
            //test3();
            test4();
            if (client != null) {
                client.close();
            }
        } catch(Exception e)
        {
            e.printStackTrace();
        }
    }

    public static void test1() throws IOException {
        String resource = "/_cat/health";
        String body = "";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        System.out.println(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
    }

    public static void test2() throws IOException {
        String resource = "knowledge_lib2/_search";
        String body = "{\n" +
                "  \"query\": {\n" +
                "    \"match\": { \"ShortCutID\": \"购物车\" }\n" +
                "  }\n" +
                "}";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        System.out.println(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
    }

    public static void test3() throws IOException {
        String resource = "knowledge_lib2/_search";
        String body = "{\n" +
                "  \"query\": {\n" +
                "    \"match\": { \"ShortCutID\": \"购物车\" }\n" +
                "  }\n" +
                "}";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        ObjectMapper mapper = new ObjectMapper();
        Map<String, Map<String, Object>> maps =
                mapper.readValue(IOUtils.toString(response.getEntity().getContent(), "UTF-8"), Map.class);
        System.out.println(maps.size());
        Set<String> key = maps.keySet();
        Iterator<String> iter = key.iterator();
        while (iter.hasNext()) {
            String field = iter.next();
            System.out.println(field + ":" + maps.get(field));
        }
    }

    public static void test4() throws IOException {
        String resource = "knowledge_lib2/_search";
        String body = "{\n" +
                "  \"query\": {\n" +
                "    \"match\": { \"ShortCutID\": \"购物车\" }\n" +
                "  }\n" +
                "}";
        Response response = client.performRequest("GET", resource, Collections.singletonMap("pretty", "true"),
                new NStringEntity(body, ContentType.APPLICATION_JSON));
        ObjectMapper mapper = new ObjectMapper();
        JsonNode rootNode = mapper.readTree(IOUtils.toString(response.getEntity().getContent(), "UTF-8"));
        int took = rootNode.path("took").asInt();
        System.out.println("took:" + took);
        JsonNode node = rootNode.path("hits");
        int total = node.path("total").asInt();
        System.out.println("hits:" + total);
        node = node.path("hits");
        for (int i = 0; i < node.size(); i++) {
            JsonNode node2 = node.get(i);
            node2 = node2.path("_source");
            String s = node2.path("ShortCutID").asText();
            System.out.println(i + ":" + s);
        }
    }
}
