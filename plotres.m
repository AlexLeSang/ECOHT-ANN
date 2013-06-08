function plotres(x)
z = x(:,3);
c = int32(sqrt(rows(z)));
zz = zeros(c,c);
a = 1;
b = c;
for i=1:c
zz(:,i) = z(a:b,1);
a += c;
b += c;
end
tx = ty = linspace (0, 1, c)';
mesh(tx,ty,zz);
end
