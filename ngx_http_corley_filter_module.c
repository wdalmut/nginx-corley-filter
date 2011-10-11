
/*
 * Copyright (C) Walter Dal Mut
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_flag_t          enable;
    ngx_str_t			keyid;
    ngx_str_t			key;
    ngx_str_t			queue_name;
} ngx_http_corley_conf_t;

static ngx_int_t ngx_http_corley_filter_init(ngx_conf_t *cf);
static void * ngx_http_corley_create_conf(ngx_conf_t *cf);
static char * ngx_http_corley_merge_conf(ngx_conf_t *cf, void *parent, void *child);
static void * ngx_http_corley_create_conf(ngx_conf_t *cf);

static ngx_command_t  ngx_http_corley_filter_commands[] = {

    { ngx_string("corley"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_corley_conf_t, enable),
      NULL },
    { ngx_string("corley_keyid"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
		ngx_conf_set_str_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_corley_conf_t, keyid),
		NULL},
	{ ngx_string("corley_key"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
		ngx_conf_set_str_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_corley_conf_t, key),
		NULL},
	{ ngx_string("corley_queue"),
		NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
		ngx_conf_set_str_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_corley_conf_t, queue_name),
		NULL},

      ngx_null_command
};


static ngx_http_module_t  ngx_http_corley_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_corley_filter_init,           /* postconfiguration */
    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */
    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */
    ngx_http_corley_create_conf,           /* create location configuration */
    ngx_http_corley_merge_conf             /* merge location configuration */
};


ngx_module_t  ngx_http_corley_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_corley_filter_module_ctx,      /* module context */
    ngx_http_corley_filter_commands,         /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;


static ngx_int_t
ngx_http_corley_header_filter(ngx_http_request_t *r)
{
    ngx_http_corley_conf_t  *conf;
    conf = ngx_http_get_module_loc_conf(r, ngx_http_corley_filter_module);

    if (!conf->enable) {
   	    return ngx_http_next_header_filter(r);
    }

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, (char *)conf->keyid.data);
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, (char *)conf->key.data);
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, (char *)conf->queue_name.data);


    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
              "Header filter engaded");

    return ngx_http_next_header_filter(r);
}


static ngx_int_t
ngx_http_corley_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{

    ngx_http_corley_conf_t  *conf;
    conf = ngx_http_get_module_loc_conf(r, ngx_http_corley_filter_module);

   if (!conf->enable) {
      return ngx_http_next_body_filter(r,in);
   }
   
   ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
              "Body filter engaged");
    return ngx_http_next_body_filter(r, in);
}

static void *
ngx_http_corley_create_conf(ngx_conf_t *cf)
{
    ngx_http_corley_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_corley_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc():
     *
     *     conf->bufs.num = 0;
     *     conf->types = { NULL };
     *     conf->types_keys = NULL;
     */

    conf->enable = NGX_CONF_UNSET;

    return conf;
}


static char *
ngx_http_corley_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_corley_conf_t *prev = parent;
    ngx_http_corley_conf_t *conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    ngx_conf_merge_str_value(conf->keyid, prev->keyid, "");
    ngx_conf_merge_str_value(conf->key, prev->key, "");
    ngx_conf_merge_str_value(conf->queue_name, prev->queue_name, "");
    
    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_corley_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_corley_header_filter;

    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_corley_body_filter;

    return NGX_OK;
}
