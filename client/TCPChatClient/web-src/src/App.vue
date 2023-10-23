<script setup>
import { ref } from 'vue';
const greeting = ref("")

const update_greeting = (_greeting) => {
  greeting.value = _greeting
}
const click = () => {
  eel.get_greeting("user")(update_greeting)
  eel.hello_world()
  console.log("clicked")
}

const send = (message) => {
  eel.send_data(message)
}

const registerMessage = JSON.stringify(
  {
    'action': 'register',
    'creds': {
      'username': 'user1',
      'password': 'password1'
    }
  }
)

const loginMessage = JSON.stringify(
  {
    'action': 'login',
    'creds': {
      'username': 'user1',
      'password': 'password1'
    }
  }
)

</script>

<template>
  <div>
    <p>{{ greeting }}</p>
    <button type="button" @click="send(loginMessage)">Login</button>
    <button type="button" @click="send(registerMessage)">Register</button>
  </div>
</template>
