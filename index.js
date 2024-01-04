let members = [];

function insertcast() {
  let input = document.getElementById('cast');
  let member = input.value.trim();
  console.log(`you entered: ${member}`);
  if (member !== '') {
    members.push(member);
    input.value = '';

    let castList = document.getElementById('castList');
    let listItem = document.createElement('li');
    listItem.textContent = member;
    castList.appendChild(listItem);
  }
}

function repop_cast() {
  let castMembersInput = document.getElementById('castInput');
  castMembersInput.value = members.join(',');
  console.log(castMembersInput.value);
}

document.getElementById('addFilmForm').addEventListener('submit',
  function (event) {
    repop_cast();
  }
);
