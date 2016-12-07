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
package com.himanshu.poc.jaxws.service.rpc_type;

import java.util.List;
import java.util.Map;

import javax.annotation.Resource;
import javax.jws.WebMethod;
import javax.jws.WebService;
import javax.xml.ws.WebServiceContext;
import javax.xml.ws.handler.MessageContext;

@WebService(endpointInterface="com.himanshu.poc.jaxws.service.rpc_type.HelloWorldService")
public class HelloWorldServiceImpl implements HelloWorldService {

	@Resource
    WebServiceContext wsctx;

	@Override
	public String getHelloWorldAsString(String name) {
		return "Hello my dear RPC friend "+name;
	}

	@Override
	public String secureHelloWorldAsString(String name) {

		MessageContext mctx = wsctx.getMessageContext();

		//get detail from request headers
        Map http_headers = (Map) mctx.get(MessageContext.HTTP_REQUEST_HEADERS);
        List userList = (List) http_headers.get("Username");
        List passList = (List) http_headers.get("Password");

        String username = "";
        String password = "";

        if(userList!=null){
        	//get username
        	username = userList.get(0).toString();
        }

        if(passList!=null){
        	//get password
        	password = passList.get(0).toString();
        }

        //Should validate username and password with database
        if (username.equals("mkyong") && password.equals("password")){
        	return "Hello World JAX-WS - Valid User!";
        }else{
        	return "Unknown User!";
        }
	}

}
