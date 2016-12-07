/*
 * Copyright 2013 Himanshu Bhardwaj
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
*/
package com.himanshu.poc.jaxws.document_client;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.xml.ws.BindingProvider;
import javax.xml.ws.handler.MessageContext;

import com.himanshu.poc.jaxws.document_client.HelloWorldServiceImplServiceExtension;

import gen.com.himanshu.poc.jaxws.stubs.document_type.HelloWorldService;
import gen.com.himanshu.poc.jaxws.stubs.document_type.HelloWorldServiceImplService;


public class DocumentClient {

	private static String WS_URL = "http://localhost:9999/ws/hellodocument"; //This is how we can make it configurable
	public static void main(String[] args) {
		HelloWorldServiceImplService helloWorld = new HelloWorldServiceImplService();
		HelloWorldService helloWorldService = helloWorld.getHelloWorldServiceImplPort();

		//Case-1 normal web-service call
		System.out.println(helloWorldService.getHelloWorldAsString(" DOCUMENT MY FRIEND"));

		//Case-2 secure web-service with no credentials FAIL TEST CASE
		System.out.println(helloWorldService.secureHelloWorldAsString(" UNKNOWN AUTH SECURE DOCUMENT MY FRIEND"));


		//Case-3 secure web-service with credentials SHOULD PASS
		/*******************UserName & Password ******************************/
        Map<String, Object> req_ctx = ((BindingProvider)helloWorldService).getRequestContext();
        req_ctx.put(BindingProvider.ENDPOINT_ADDRESS_PROPERTY, WS_URL);

        Map<String, List<String>> headers = new HashMap<String, List<String>>();
        headers.put("Username", Collections.singletonList("mkyong"));
        headers.put("Password", Collections.singletonList("password"));
        req_ctx.put(MessageContext.HTTP_REQUEST_HEADERS, headers);
        System.out.println(helloWorldService.secureHelloWorldAsString(" SECURE DOCUMENT MY FRIEND"));
        /**********************************************************************/


        //Check here I have made the url configurable
        HelloWorldServiceImplServiceExtension ext = new HelloWorldServiceImplServiceExtension();
		HelloWorldService helloWorldServiceNew = ext.getPort(HelloWorldService.class);
		System.out.println(helloWorldServiceNew.getHelloWorldAsString("TTT"));

	}

}
