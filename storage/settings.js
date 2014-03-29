count = 0;

h1 = new http.Server();
h1.onrequest = function (request) {

	var r = request;
	print(request);
	print(request.constructor);
	print(request.constructor.prototype);
	
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

    var res = new http.Response();
	print(res);
    print(res.statusCode);
    res.statusCode = 500;
    print(res.statusCode);
    res.statusCode = 200;
    print(res.statusCode);

    
    res.setHeader('A', 'B');
    print(res.getHeader);
    print(res.getHeader('A'));
    res.removeHeader('A');
    print(res.getHeader('A'));

    print(res.data);
    res.data = 'A';
    print(res.data);
    res.data = 'B';
    print(res.data);
    res.data = 'C';
    print(res.data);

    return res;
    //return "ABC";
    //return null;
    //return undefined;
    //return NaN;
    //return http.Response;
};
h1.onmessage = function () {
	print('message');
};
h1.onerror = function (request) {
	print('error');
};
print(h1.listen(80));


print(h1)
print(h1.constructor);
print(h1.constructor.prototpye);