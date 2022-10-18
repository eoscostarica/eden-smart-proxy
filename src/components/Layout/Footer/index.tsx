import { Typography, Link } from '@mui/material'
import { Footer } from '@edenia/ui-kit'
import Image from 'next/image'
import { useTheme } from '@mui/styles'

import { constantConfig } from 'config'
import edeniaLogo from '/public/logos/edenia-isotipo-grey.png'

import Styles from './styles'

const useStyles = Styles

type FooterCompType = {
  showWhite: boolean
}

const FooterComp: React.FC<FooterCompType> = ({ showWhite }) => {
  const classes = useStyles()
  const theme = useTheme()

  return (
    <div className={classes.footerRoot}>
      <Footer
        socialMediaItems={
          !showWhite ? constantConfig?.footer?.socialMediaItems : undefined
        }
        buttomContent={
          <div className={classes.footerContainer}>
            <div className={classes.paddinR}>
              <Image src={edeniaLogo} />
            </div>
            <Typography variant='caption' color={theme.palette.grey[600]}>
              <Link href='https://edenia.com' color={theme.palette.grey[600]}>
                {`Hosted by Edenia `}
              </Link>
              - Community Owned
            </Typography>
          </div>
        }
        itemsFooter={constantConfig?.footer?.footerItems}
        bgColor={!showWhite ? '#343434' : '#FFFFFF'}
        color={!showWhite ? '#FFFFFF' : '#262626'}
      />
    </div>
  )
}

export default FooterComp
