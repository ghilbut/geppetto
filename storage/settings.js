h1 = new http.Server();
h1.onrequest = function (request) {
	print(request);
	print(request.method);
}
h1.onmessage = function () {
	print('message');
}
h1.onerror = function (request) {
	print('error');
}
print(h1.listen('a', 8080));