
var CAT = 0



async function FrankHealth(){

    var resp = await axios.get('/test/frank-health')

    
    var resp_data = resp.data

    message = resp_data['health']

    RenderOutput('frank-health',message)

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


