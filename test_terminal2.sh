echo -e "\e[H\e[J"

echo -e "\e[10;5H\u250C"
echo -e "\e[10;12H\u2510"
echo -e "\e[18;5H\u2514"
echo -e "\e[18;12H\u2518"

for i in {6..11}
do
    echo -e "\e[10;${i}H\u2500"
    echo -e "\e[18;${i}H\u2500"
done

for i in {11..17}
do
    echo -e "\e[${i};5H\u2502"
    echo -e "\e[${i};12H\u2502"
done

echo -e "\e[12;6H\u2587"
echo -e "\e[11;7H\u2583"
echo -e "\e[11;8H\u2583"
echo -e "\e[11;9H\u2583"
echo -e "\e[12;10H\u2587"
echo -e "\e[13;11H\u258C"
echo -e "\e[14;10H\u2588"
echo -e "\e[14;9H\u2588"
echo -e "\e[14;8H\u2588"
echo -e "\e[15;11H\u258C"
echo -e "\e[16;10H\u2587"
echo -e "\e[17;9H\u2580"
echo -e "\e[17;8H\u2580"
echo -e "\e[17;7H\u2580"
echo -e "\e[16;6H\u2587"

echo -e "\e[19;0H"