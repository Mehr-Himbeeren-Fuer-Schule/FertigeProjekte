function read_TEMP() {
  var html = "";
  for(var i = 0;i < temp_data.length;i++) {
    html += "<h1> " + temp_data[i][0] + "</h1><table>";
    for(var j = 0;j <  temp_data[i][1].length;j++) {
      if(j % 6 == 0) {
        html += "</tr><tr>";
      }
      html += "<td>" + temp_data[i][1][j] + "</td>";
    }
    html += "</tr></table>";
  }
  return html;
}

function init() {
  var background = document.getElementById("main");
  var html = read_TEMP();
  background.innerHTML = html;
}
init();