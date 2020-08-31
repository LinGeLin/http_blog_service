# http_blog_service

# 增加博客

```
post /blog

body 

{
  "title" : "title",
  "content" : "content",
  "tag_id" : tag_id,
  "create_time" : "create_time",
}

res

{
  "ok" : true
}
```

# 删除博客

```
delete /blog?blog_id=blog_id

res

{
  "ok" : true
}
```