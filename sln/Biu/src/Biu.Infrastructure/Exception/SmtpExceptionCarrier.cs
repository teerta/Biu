using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.ComponentModel;
using System.Net;
using System.Net.Mail;
using System.Net.Mime;

namespace Biu
{
    public class SmtpExceptionCarrier : ExceptionCarrierBase
    {
        SmtpClient _client = null;
        private string _strStmpHost;
        private string _strMailAddress;
        private string _strDestMailAddress;
        private string _strDisplayName;
        private string _strPassword;

        public SmtpExceptionCarrier(string strSmtpHost, string strMailAddress, string strPassword, string strDisplayName)
            : base()
        {
            _strStmpHost = strSmtpHost;
            _strMailAddress = strMailAddress;
            _strDestMailAddress = strMailAddress;
            _strPassword = strPassword;
            _strDisplayName = strDisplayName;

            try
            {
                _client = new SmtpClient(_strStmpHost);
                _client.UseDefaultCredentials = false;
                _client.Credentials = new NetworkCredential(_strMailAddress, _strPassword);
                _client.DeliveryMethod = SmtpDeliveryMethod.Network;
                _client.EnableSsl = true;
            }
            catch
            {
                throw;
            }
        }

        private void SendCompletedCallback(object sender, AsyncCompletedEventArgs e)
        {
            try
            {
                if (e.Cancelled)
                {
                    this.Sent = false;
                }
                else
                {
                    if (null == e.Error)
                    {
                        this.Sent = true;
                    }
                    else
                    {
                        this.Sent = false;
                        this.LastError = e.Error.ToString();
                    }
                }
            }
            catch
            {
                throw;
            }
            finally
            {
                this.Running = false;
                SetEvent();
                OnSendCompleted();
            }
        }

        public override bool Send(string subject, string content)
        {
            if (null == _client)
            {
                return false;
            }

            bool bRet = false;
            this.Sent = false;

            MailAddress from = new MailAddress(_strMailAddress, _strDisplayName, Encoding.UTF8);
            MailAddress to = new MailAddress(_strDestMailAddress);
            
            MailMessage message = new MailMessage(from, to);
            
            try
            {
                RecreateEvent(true);

                message.Body =  DateTime.Now.ToString() + Environment.NewLine + Environment.NewLine + content;
                message.BodyEncoding = System.Text.Encoding.UTF8;
                message.Subject = subject;
                message.SubjectEncoding = System.Text.Encoding.UTF8;

                _client.SendCompleted += new SendCompletedEventHandler(SendCompletedCallback);

                this.Running = true;

                _client.SendAsync(message, null);

                bRet = true;
            }
            catch
            {
                throw;
            }

            return bRet;
        }

        public override void CancelSend()
        {
            if (null == _client)
            {
                return;
            }

            if (!this.Running)
            {
                return;
            }

            try
            {
                _client.SendAsyncCancel();
            }
            catch
            {
                throw;
            }
        }
    }
}
