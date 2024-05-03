
from views import index as views_index



def Init(app):

    
    app.add_url_rule('/',methods=['GET'],view_func=views_index.Index)
    
    app.add_url_rule('/test/frank-health',methods=['GET'], view_func=views_index.HealthCheck)

    app.add_url_rule('/authorize/pub',methods=['POST'], view_func=views_index.AuthorizePubKey)

    app.add_url_rule('/oauth2/google/login',methods=['GET'], view_func=views_index.OauthLogin)

    app.add_url_rule('/oauth2/google/callback',methods=['GET'], view_func=views_index.OauthLoginCallback)

    return app