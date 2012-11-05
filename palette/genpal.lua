print "GIMP Palette"
print "Name: U23 board"
print "#"

for r = 0, (2^3-1) do
	for g = 0, (2^3-1) do
		for b = 0, (2^2-1) do
			print(tostring(256/(2^3) * r) .. " " .. tostring(256/(2^3) * g) .. " " .. tostring(256/(2^2) * b))
		end
	end
end
