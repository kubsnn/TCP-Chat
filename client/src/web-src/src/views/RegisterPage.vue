<script setup>
import { FwbButton, FwbHeading, FwbInput } from 'flowbite-vue'

</script>

<script>
export default {
    data() {
        return {
            username: '',
            password: '',
            repeatPassword: '',
            loading: false,
            validUsername: true,
            validPassword: true,
            validUsernameMessage: '',
            validPasswordMessage: ''
        };
    },
    methods: {
        onRegister() {
            try {

                if (this.validateUsernameLen() == false) {
                    return
                }

                this.validUsername = true
                this.validUsernameMessage = ''


                if (this.validatePasswordMatch() == false) {
                    return
                }
                this.validPassword = true
                this.validPasswordMessage = ''

                if (this.validPasswordComplexity() == false) {
                    return
                }

                this.validPassword = true
                this.validPasswordMessage = ''



                //set loading to true
                this.loading = true


                eel.register_to_server(this.username, this.password)(this.handleResponse);


            } catch (error) {
                console.log(error)

            }
        },
        changeRoute(newRoute) {
            this.$router.push(newRoute);

        },
        validatePasswordMatch() {
            if (this.password != this.repeatPassword) {
                this.validPassword = false
                this.validPasswordMessage = 'Passwords do not match'
                return false
            }
            return true
        },
        validPasswordComplexity() {
            if (this.password.length < 8) {
                this.validPassword = false
                this.validPasswordMessage = 'Password must be at least 8 characters long'
                return false
            }
            return true
        },
        validateUsernameLen() {
            if (this.username.length < 3) {
                this.validUsername = false
                this.validUsernameMessage = 'Username must be at least 3 characters long'
                return false
            }
            return true
        },
        handleResponse(response) {

            try {
                if (response == true) {
                    this.loading = false
                    eventBus.emit('show-toast', 'success', 'Successfully registered', 5000);
                    this.$router.push('/chat')
                }
                else {
                    this.loading = false
                    eventBus.emit('show-toast', 'danger', 'Error registering', 5000);
                }
            }
            catch (error) {
                console.log(error)
                eventBus.emit('show-toast', 'danger', 'Error registering', 5000);
                this.loading = false
                return
            }

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
            <fwb-heading class="text-center" tag="h2">Register</fwb-heading>
            <fwb-input v-if="validUsername" v-model="username" required placeholder="enter your username"
                label="Username" />
            <fwb-input v-else v-model="username" required placeholder="enter your username" label="Username"
                validation-status="error">
                <template #validationMessage>
                    {{ validUsernameMessage }}
                </template>
            </fwb-input>
            <fwb-input v-if="validPassword" v-model="password" type="password" required placeholder="enter your password"
                label="Password" />
            <fwb-input v-else v-model="password" type="password" required placeholder="enter your password" label="Password"
                validation-status="error">
                <template #validationMessage>
                    {{ validPasswordMessage }}
                </template>
            </fwb-input>
            <fwb-input v-if="validPassword" v-model="repeatPassword" type="password" required
                placeholder="repeat your password" label="Repeat Password" />
            <fwb-input v-else v-model="repeatPassword" type="password" required placeholder="repeat your password"
                validation-status="error" label="Repeat Password" />


            <fwb-button class="mt-4 justify-center" size="lg" color="pink" @click="onRegister()">Register
                <template #suffix>
                    <font-awesome-icon v-if="loading" icon="fa-solid fa-spinner" spin-pulse />
                    <font-awesome-icon v-else icon="fa-solid fa-right-to-bracket" />
                </template>
            </fwb-button>
        </div>

    </div>
</template>