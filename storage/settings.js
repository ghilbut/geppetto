h1 = new http.Server();
h1.onrequest = function (request) {

	r = request;
	//print(request);
	//print(request.method);
	//print(request.constructor);
	//print(request.constructor.prototype);
	
	print(r.request_method);
	print(r.uri);
	print(r.http_version);
	print(r.query_string);
	print(r.remote_ip);
	print(r.local_ip);
	print(r.remote_port);
	print(r.local_port);
	var hs = r.headers;
	for (var i = 0, len = hs.length; i < len; ++i) {
		print('----------------------------------------');
		var n = hs[i].name, v = hs[i].value;
		print(hs[i]);
		print(n);
		print(v);
		print(hs[n]);
		print('----------------------------------------');
	}
	print(r.content);
}
h1.onmessage = function () {
	print('message');
}
h1.onerror = function (request) {
	print('error');
}
print(h1.listen(80));


print(h1)
print(h1.constructor);
print(h1.constructor.prototpye);