## ANN检索工具
- 安装 `make`

## HIVE或Spark中使用
- HIVE中通过 transform 函数将数据通过管道打给 annoy
- Spark 通过 PipeRDD 将数据通过管道打给 annoy

### 全局索引
```sql
select transform(word, vec) using 'path/to/annoy -size <向量维度> -nnsize <输出多少个向量> -distance <使用哪种距离度量>'
    as (word, simi_word, dist)
from (
    select word, vec
    from `TABLE`
    distribute by 1       ------ 将所有数据shuffle到1台机器
)A
```

### 分组索引
```sql
select transform(groupid, word, vec) using 'path/to/annoy -size <向量维度> -nnsize <输出多少个向量> -distance <使用哪种距离度量>'
    as (groupid, word, simi_word, dist)
from (
    select groupid, word, vec
    from `TABLE`
    cluster by groupid       ------ 将同一组数据shuffle到1台机器并且按照groupid排序
)A
```
