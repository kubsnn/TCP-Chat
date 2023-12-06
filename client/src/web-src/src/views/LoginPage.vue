<script setup>
import { FwbButton, FwbHeading, FwbInput } from 'flowbite-vue'
import { eventBus } from '@/services/EventBus';

</script>

<script>
export default {
    data() {
        return {
            username: '',
            password: '',
            validationStatus: true,
            validationMessage: '',
            loading: false
        };
    },
    methods: {
        async onLogin() {
            try {

                //set loading to true
                this.loading = true

                const loginMessage = JSON.stringify(
                    {
                        'action': 'login',
                        'creds': {
                            'username': this.username,
                            'password': this.password
                        }
                    }
                )
                eel.send_data(loginMessage);
                await eel.receive_from_server()(this.handleResponse);


            } catch (error) {
                console.log(error)
                eventBus.emit('show-toast', 'danger', 'Error logging in', 5000);
            }
        },
        handleResponse(response) {

            try {
                if (response == false) {

                    this.loading = false
                    //return error
                    throw new Error("Error receiving response from server")
                }

                //remove whitespace after and before braces
                response = response.trim()
                //remove last char
                response = response.slice(0, -1)

                //jsonify string
                response = JSON.parse(response)

                if (response["response"]) {
                    this.loading = false
                    if (response["result"] == "fail") {
                        console.log(response["message"])
                        eventBus.emit('show-toast', 'danger', response["message"], 5000);
                    }
                    else {
                        console.log(response["message"])
                        eventBus.emit('show-toast', 'success', response["message"], 5000);
                        this.$router.push('/chat')
                    }
                }
            }
            catch (error) {
                console.log(error)
                eventBus.emit('show-toast', 'danger', 'Error logging in', 5000);
                this.loading = false
                return
            }

        },
        changeRoute(newRoute) {
            this.$router.push(newRoute);

        },
    },

};
</script>

<template>
    <div class="p-4 absolute">
        <fwb-button color="pink" pill square @click="changeRoute('/')">
            <font-awesome-icon class="w-5 h-4" icon="fa-solid fa-arrow-left" />
        </fwb-button>
    </div>
    <div class="flex justify-center align-middle w-full h-full">

        <div class="form flex flex-col gap-4 justify-center w-2/3">
            <fwb-heading class="text-center" tag="h2">Login</fwb-heading>
            <fwb-input v-model="username" required placeholder="enter your username" label="Username" />
            <fwb-input v-if="validationStatus" v-model="password" type="password" required placeholder="enter your password"
                label="Password" />
            <fwb-input v-else v-model="password" type="password" required placeholder="enter your password" label="Password"
                validation-status="error">
                <template #validationMessage>
                    {{ validationMessage }}
                </template>
            </fwb-input>
            <fwb-button class="mt-4 justify-center" size="lg" color="pink" @click="onLogin()">Login
                <template #suffix>
                    <font-awesome-icon v-if="loading" icon="fa-solid fa-spinner" spin-pulse />
                    <font-awesome-icon v-else icon="fa-solid fa-right-to-bracket" />
                </template>
            </fwb-button>
        </div>

    </div>
</template>