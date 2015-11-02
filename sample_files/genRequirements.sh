
#!/bin/bash

################################################# 
# Make a custom load balanced requirements file #
#                                               #
# APR: 07/05/13                                 #
#################################################

# # Set number of runs
# echo "1530" > req1530.txt

# # FastPC2
# for i in {1..203}
# do
# echo " Machine == \"fastpc2\" &&  Arch == \"INTEL\"" >> req1530.txt
# done

# # FastPC3
# for i in {1..207}
# do
# echo " Machine == \"fastpc3\" &&  Arch == \"INTEL\"" >> req1530.txt
# done

# # FastPC4
# for i in {1..207}
# do
# echo " Machine == \"fastpc4\" && Arch == \"INTEL\"" >> req1530.txt
# done

# # FastPC5
# for i in {1..207}
# do
# echo " Machine == \"fastpc5\" && Arch == \"INTEL\"" >> req1530.txt
# done

# # FastPC6
# for i in {1..440}
# do
# echo " Machine == \"fastpc6\" && Arch == \"x86_64\"" >> req1530.txt
# done

# # FastPC8
# for i in {1..266}
# do
# echo " Machine == \"fastpc8\" && Arch == \"x86_64\"" >> req1530.txt
# done


# # Set number of runs
# echo "240" > req240.txt

# # FastPC2
# for i in {1..32}
# do
# echo " Machine == \"fastpc2\" &&  Arch == \"INTEL\"" >> req240.txt
# done

# # FastPC3
# for i in {1..33}
# do
# echo " Machine == \"fastpc3\" &&  Arch == \"INTEL\"" >> req240.txt
# done

# # FastPC4
# for i in {1..33}
# do
# echo " Machine == \"fastpc4\" && Arch == \"INTEL\"" >> req240.txt
# done

# # FastPC5
# for i in {1..33}
# do
# echo " Machine == \"fastpc5\" && Arch == \"INTEL\"" >> req240.txt
# done

# # FastPC6
# for i in {1..69}
# do
# echo " Machine == \"fastpc6\" && Arch == \"x86_64\"" >> req240.txt
# done

# # FastPC8
# for i in {1..40}
# do
# echo " Machine == \"fastpc8\" && Arch == \"x86_64\"" >> req240.txt
# done


# Set number of runs
echo "240" > req240std.txt

# FastPC2
for i in {1..16}
do
echo " Machine == \"fastpc2\" &&  Arch == \"INTEL\"" >> req240std.txt
done

# FastPC3
for i in {1..16}
do
echo " Machine == \"fastpc3\" &&  Arch == \"INTEL\"" >> req240std.txt
done

# FastPC4
for i in {1..16}
do
echo " Machine == \"fastpc4\" && Arch == \"INTEL\"" >> req240std.txt
done

# FastPC5
for i in {1..16}
do
echo " Machine == \"fastpc5\" && Arch == \"INTEL\"" >> req240std.txt
done

# FastPC6
for i in {1..96}
do
echo " Machine == \"fastpc6\" && Arch == \"x86_64\"" >> req240std.txt
done

# FastPC8
for i in {1..80}
do
echo " Machine == \"fastpc8\" && Arch == \"x86_64\"" >> req240std.txt
done



 
