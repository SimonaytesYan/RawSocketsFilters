## How to compile and run

To compile on alpine run first
```
sh scripts/configure_alpine.sh
```

To compile run
```
make
```

To run on alpine
```
make alpine
```
or
```
bin/run first_interface_ind second_interface_ind filter_rule_file_path
```
where `first_interface_ind` and `second_interface_ind` are some numbers, `filter_file_path` - path ot filter rule file


## Filter rules file format
* Every rule is one line
* To specify pass or delete package, type `-` or `+` with first symbol
* To specify limit type name of param and it's value:
```
src_port=80 dst_port=80 src_ip=10.0.1.2
```
* To specify black or white list, on the last line type `-` or `+`

Example you can see in file `rules.fr`
