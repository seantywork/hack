
REQ_KEY = ""

var CAT = 0


async function FrankHealth(){

    let resp = await axios.get('/healthz')

    
    let resp_data = resp.data

    if(resp_data.status != 'SUCCESS'){
 
      alert('failed to get health: ' + resp_data.data)
 
    } else {
 
      message = resp_data.data

      RenderOutput('frank-health',message)

    }


}


async function SignOut(){


    var resp = await axios.get('/signout')

    var resp_data = resp.data

    message = resp_data.status

    if(message != 'SUCCESS'){
      alert('sign out failed')
    } else {

      alert('sign out success')

      location.href = '/'

    }



}

function Delay(time) {
  return new Promise(resolve => setTimeout(resolve, time));
}

function RenderOutput(output_name, output_data){


  var result_html  = `
    <thead>
      <tr>
        <th> ${output_name} </th>
      </tr>
    </thead>
    <tbody id="app-result-render-body">
    </tbody>
  `

  $('#app-result-render').html(result_html)


  var body_html = `
    <tr>
        <td> ${output_data} </td>
    </tr>
  `

  $('#app-result-render-body').append(body_html)

  if (CAT == 0){
    $('#app-img-switch').html('<img src="/static/img/super_cat_hunt.jpg" class="rounded-circle">')
    CAT = 1
  } else{
    $('#app-img-switch').html('<img src="/static/img/super_cat.jpg" class="rounded-circle">')
    CAT = 0
  }
}


function getCookie(name) {
  const value = `; ${document.cookie}`
  const parts = value.split(`; ${name}=`)
  if (parts.length === 2) {
    return parts.pop().split(';').shift()
  } else {
    return null
  }
}



async function _channel_init(){


  FRONT_WS = FrontWSInit()

  console.log('front web socket ready')

  await Delay(1000)
  

  REQ_KEY = getCookie("access_token")


  console.log("ready commmunication")

}

_channel_init()